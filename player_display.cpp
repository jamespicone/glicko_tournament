#include "player_display.h"

#include <QStandardItem>
#include <QStringList>
#include <QObject>
#include <QHeaderView>
#include <QStandardItemModel>

static const int NUM_COLUMNS = 6;

static const int COLUMN_NAME = 0;
static const int COLUMN_RATING = 1;
static const int COLUMN_RD = 2;
static const int COLUMN_ID = 3;
static const int COLUMN_WINS = 4;
static const int COLUMN_LOSSES = 5;

PlayerDisplayWindow::PlayerDisplayWindow(QWidget* parent)
:QMainWindow(parent),
SortIndex(0), SortOrder(Qt::AscendingOrder)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	Table = new QTableView(this);
	Model = new QStandardItemModel(this);
	
	setCentralWidget(Table);
	
	QStringList headers;
	headers << "Name" << "Effective Rating" << "RD" << "ID" << "Wins" << "Losses";
	
	Model->setColumnCount(NUM_COLUMNS);
	Model->setHorizontalHeaderLabels(headers);
	
	Model->setSortRole(Qt::UserRole);
	
	Table->verticalHeader()->hide();
	Table->setModel(Model);
	
	Table->setSortingEnabled(true);
	
	QObject::connect(Table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sortChanged(int, Qt::SortOrder)));
}

PlayerDisplayWindow::~PlayerDisplayWindow()
{
}

void PlayerDisplayWindow::sortChanged(int index, Qt::SortOrder order)
{
	SortIndex = index;
	SortOrder = order;
}

void PlayerDisplayWindow::update_data(QList<Player> data)
{
	//Model->clear();
	Model->setRowCount(data.size());
	
	/*
	QStringList headers;
	headers << "Name" << "Effective Rating" << "RD" << "ID" << "Wins" << "Losses";
	
	Model->setColumnCount(NUM_COLUMNS);
	Model->setHorizontalHeaderLabels(headers);
	Table->verticalHeader()->hide();*/
	
	int row = 0;
	
	foreach (Player p, data)
	{
		if (!Model->item(row, COLUMN_NAME))
		{
			QList<QStandardItem*> items_list;
			for (int i = 0; i < NUM_COLUMNS; ++i) {items_list += new QStandardItem();}
			Model->insertRow(row, items_list);
		}
		
		Model->item(row, COLUMN_NAME)->setText(p.Name);
		Model->item(row, COLUMN_NAME)->setData(p.Name, Qt::UserRole);
		
		Model->item(row, COLUMN_RATING)->setText(QString::number(p.Rating - p.RD, 'f', 2));
		Model->item(row, COLUMN_RATING)->setData(p.Rating - p.RD, Qt::UserRole);
		
		Model->item(row, COLUMN_RD)->setText(QString::number(p.RD, 'f', 2));
		Model->item(row, COLUMN_RD)->setData(p.RD, Qt::UserRole);
		
		Model->item(row, COLUMN_ID)->setText(QString::number(p.Id));
		Model->item(row, COLUMN_ID)->setData(p.Id, Qt::UserRole);
		
		Model->item(row, COLUMN_WINS)->setText(QString::number(p.Wincount));
		Model->item(row, COLUMN_WINS)->setData(p.Wincount, Qt::UserRole);
		
		Model->item(row, COLUMN_LOSSES)->setText(QString::number(p.Losscount));
		Model->item(row, COLUMN_LOSSES)->setData(p.Losscount, Qt::UserRole);
		
		row++;
	}
	
	Table->sortByColumn(SortIndex, SortOrder);
}