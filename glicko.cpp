#include <QMenuBar>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

#include "glicko.h"
#include "player_display.h"

TournamentControlPanel::TournamentControlPanel()
:QMainWindow()
{
	QWidget* central = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout();
	central->setLayout(layout);
	setCentralWidget(central);
	
	Output = new QTextEdit(central);
	Output->setReadOnly(true);
	layout->addWidget(Output);
	
	OutputFile = "temp.txt";
}

void TournamentControlPanel::CreateMenubar()
{
	QMenuBar* menu_bar = new QMenuBar(this);

	QMenu* menu = new QMenu("&File", menu_bar);
	QAction* close_action = menu->addAction("&Quit");
	menu_bar->addMenu(menu);
	
	menu = new QMenu("&Player Management", menu_bar);
	QAction* add_action = menu->addAction("Add &Player");
	QAction* batch_action = menu->addAction("Add &Batch");
	QAction* remove_action = menu->addAction("&Remove Player");
	QAction* export_action = menu->addAction("&Export Players");
	menu_bar->addMenu(menu);
	
	menu = new QMenu("&Results", menu_bar);
	QAction* result_action = menu->addAction("Add &Result");
	QAction* batch_result_action = menu->addAction("Add &Batch Result");
	QAction* undo_action = menu->addAction("&Undo");
	QAction* set_output_action = menu->addAction("Set &Output File");
	menu_bar->addMenu(menu);
	
	menu = new QMenu("&Windows", menu_bar);
	QAction* new_action = menu->addAction("New &Window");
	menu_bar->addMenu(menu);
	
	setMenuBar(menu_bar);
	
	close_action->setShortcut(QKeySequence("Ctrl+x"));
	result_action->setShortcut(QKeySequence("Ctrl+r"));
	new_action->setShortcut(QKeySequence("Ctrl+w"));
	undo_action->setShortcut(QKeySequence("Ctrl+z"));
	
	connect(add_action, SIGNAL(triggered()), this, SLOT(add_player()));
	connect(batch_action, SIGNAL(triggered()), this, SLOT(add_player_batch()));
	connect(remove_action, SIGNAL(triggered()), this, SLOT(remove_player()));
	connect(new_action, SIGNAL(triggered()), this, SLOT(create_new_window()));
	connect(close_action, SIGNAL(triggered()), this, SLOT(close()));
	connect(result_action, SIGNAL(triggered()), this, SLOT(add_result()));
	connect(batch_result_action, SIGNAL(triggered()), this, SLOT(add_batch_result()));
	connect(set_output_action, SIGNAL(triggered()), this, SLOT(set_output_file()));
	connect(undo_action, SIGNAL(triggered()), this, SLOT(undo()));
	connect(export_action, SIGNAL(triggered()), this, SLOT(export_players()));
}

void TournamentControlPanel::set_output_file()
{
	bool ok = false;
	
	QString filename = QInputDialog::getText(this, "Set Output File", "File name?", QLineEdit::Normal, "", &ok);
	if (!ok) {Output->append("Output file set cancelled"); return;}
	
	OutputFile = filename;
	Output->append("Output file set to " + filename);
}

void TournamentControlPanel::undo(bool log)
{
	foreach (Player p, UndoBuffer)
	{
		Output->append(QString("Undoing change to player ") + QString::number(p.Id) + ", " + p.Name);
		Players[p.Id] = p;
	}
	
	UndoBuffer.clear();
	
	if (log)
	{
		QFile outputFile(OutputFile);
		if (outputFile.open(QFile::WriteOnly | QFile::Append | QIODevice::Text))
		{
			QTextStream out(&outputFile);
			out << "undo" << endl;
		}
	}
	
	emit update_data(Players.values());
}

void TournamentControlPanel::add_result()
{
	bool ok = false;
	
	int win_id = QInputDialog::getInt(this, "Add Result", "Winner ID?", 0, 0, 127, 1, &ok);
	if (!ok) {Output->append("Result add cancelled"); return;}
	
	int lose_id = QInputDialog::getInt(this, "Add Result", "Loser ID?", 0, 0, 127, 1, &ok);
	if (!ok) {Output->append("Result add cancelled"); return;}
	
	if (win_id == lose_id)
	{
		Output->append("Winner and loser ID are the same. Result add cancelled"); return;
	}
	
	if (Players.count(win_id) <= 0) {Output->append("Couldn't find winning player, cancelled"); return;}
	if (Players.count(lose_id) <= 0) {Output->append("Couldn't find losing player, cancelled"); return;}

	Player* win_player = &Players[win_id];
	Player* lose_player = &Players[lose_id];
	
	QFile outputFile(OutputFile);
	if (outputFile.open(QFile::WriteOnly | QFile::Append | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		out << win_id << " beats " << lose_id << endl;
	}
	
	UndoBuffer.clear();
	UndoBuffer.push_back(*win_player); UndoBuffer.push_back(*lose_player);
	
	Output->append("Adding game " + QString::number(win_id) + " beats " + QString::number(lose_id));
	
	if(!Player::UpdatePlayers(win_player, lose_player))
	{
		Output->append("Those two players have already played each other. The game has not been counted");
	}
	
	emit update_data(Players.values());
}

void TournamentControlPanel::add_batch_result()
{
	bool ok = false;
	
	QString file_name = QInputDialog::getText(this, "Add Batch", "Batch file?", QLineEdit::Normal, "", &ok);
	if (!ok) {Output->append("Batch result add cancelled"); return;}
	
	QFile file(file_name);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		Output->append("Couldn't open file " + file_name); return;
	}
	
	QTextStream in(&file);
	
	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line == QString("undo"))
		{
			undo(false);
		}
		else		
		{
			QStringList data = line.split(" beats ");
			
			if (data.size() != 2) {Output->append("Batch file " + file_name + " is not a valid results batch file"); return;}
			
			int win_id = data[0].toInt();
			int lose_id = data[1].toInt();
			
			if (win_id == lose_id)
			{
				Output->append("Winner and loser ID are the same. Result add cancelled"); return;
			}
			
			if (Players.count(win_id) <= 0) {Output->append("Couldn't find winning player, cancelled"); return;}
			if (Players.count(lose_id) <= 0) {Output->append("Couldn't find losing player, cancelled"); return;}

			Player* win_player = &Players[win_id];
			Player* lose_player = &Players[lose_id];
			
			UndoBuffer.clear();
			UndoBuffer.push_back(*win_player); UndoBuffer.push_back(*lose_player);
	
			if(!Player::UpdatePlayers(win_player, lose_player))
			{
				Output->append("The players " + win_player->Name + " and " + lose_player->Name + " have already played each other");
			}
			else
			{
				Output->append("Added result " + win_player->Name + " beats " + lose_player->Name);
			}
		}
	}
	
	emit update_data(Players.values());
	Output->append("Batch results done");
}

void TournamentControlPanel::add_player()
{	bool ok = false;
	
	QString name = QInputDialog::getText(this, "Add Player", "Player name?", QLineEdit::Normal, "", &ok);
	if (!ok) {Output->append("Player add cancelled"); return;}
	
	int id = QInputDialog::getInt(this, "Add Player", "Player ID?", 0, 0, 127, 1, &ok);
	if (!ok) {Output->append("Player add cancelled"); return;}
	
	if (Players.count(id) > 0)
	{
		Output->append("Duplicate ID, player add cancelled");
		return;
	}
	
	Output->append("Player " + name + " added");
	
	Players.insert(id, Player(name, id));
	emit update_data(Players.values());
}

void TournamentControlPanel::add_player_batch()
{
	bool ok = false;
	
	QString file_name = QInputDialog::getText(this, "Add Batch", "Batch file?", QLineEdit::Normal, "", &ok);
	if (!ok) {Output->append("Batch player add cancelled"); return;}
	
	QFile file(file_name);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		Output->append("Couldn't open file " + file_name); return;
	}
	
	QTextStream in(&file);
	
	while (!in.atEnd())
	{
		QString line = in.readLine();
		
		{
			QStringList data = line.split("|");
			
			if (data.size() != 2) {Output->append("Batch file " + file_name + " is not a valid batch file"); return;}
			
			QString name = data[0];
			int id = data[1].toInt();
			
			if (Players.count(id) > 0) {Output->append("Batch file " + file_name + " contains duplicate id for entry " + name + ", skipping"); continue;}
			if (id < 0 || id > 127) {Output->append("Batch file " + file_name + " contains invalid id for entry " + name + ", skipping"); continue;}
			
			Players.insert(id, Player(name, id));
			Output->append("Added player " + name + " from batch file " + file_name);
		}
	}
	
	emit update_data(Players.values());
	Output->append("Batch update done");
}

void TournamentControlPanel::remove_player()
{
	emit update_data(Players.values());
	Output->append("Currently unimplemented");
}

void TournamentControlPanel::create_new_window()
{
	PlayerDisplayWindow* window = new PlayerDisplayWindow(this);
	connect(this, SIGNAL(update_data(QList<Player>)), window, SLOT(update_data(QList<Player>)));
	window->show();
	
	emit update_data(Players.values());
}

void TournamentControlPanel::export_players()
{
	bool ok = false;
	
	QString file_name = QInputDialog::getText(this, "Export Players", "Export file?", QLineEdit::Normal, "", &ok);
	if (!ok) {Output->append("Player export cancelled"); return;}
	
	QFile file(file_name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		Output->append("Couldn't open file " + file_name); return;
	}
	
	QTextStream out(&file);
	
	QList<Player> players = Players.values();
	qSort(players.begin(), players.end(), qGreater<Player>());
	
	foreach(Player p, players)
	{
		out << p << endl;
	}
}