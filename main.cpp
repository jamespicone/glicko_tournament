#include "glicko.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	TournamentControlPanel tcp;
	tcp.CreateMenubar();
	tcp.show();
    return a.exec();
}
