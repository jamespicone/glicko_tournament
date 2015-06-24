#ifndef PLAYER_DISPLAY_H__
#define PLAYER_DISPLAY_H__

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QList>

#include "player.h"

class PlayerDisplayWindow : public QMainWindow
{
	Q_OBJECT
	
public:

	PlayerDisplayWindow(QWidget* parent = 0);
	~PlayerDisplayWindow();
	
public slots:

	void update_data(QList<Player> data);
	void sortChanged(int index, Qt::SortOrder order);
	
private:
	
	int SortIndex;
	Qt::SortOrder SortOrder;
	
	QTableView* Table;
	QStandardItemModel* Model;
};


#endif