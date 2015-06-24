#ifndef GLICKO_H__
#define GLICKO_H__

#include <QString>
#include <QMainWindow>
#include <QMap>
#include <QTextEdit>

#include "player.h"

class TournamentControlPanel : public QMainWindow
{
	Q_OBJECT
	
public:
	
	TournamentControlPanel();
	void CreateMenubar();
	
public slots:
	void add_player();
	void remove_player();
	void add_player_batch();
	void export_players();
	void create_new_window();
	void add_result();
	void add_batch_result();
	void set_output_file();
	void undo(bool add_to_file = true);
	
signals:

	void update_data(QList<Player> data);
	
private:

	QMap<int, Player> Players;
	QTextEdit* Output;
	QString OutputFile;
	QList<Player> UndoBuffer;
	
};

#endif