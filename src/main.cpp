#include <QApplication>
#include <stdio.h>
#include <QFont>
#include <QFontDatabase>
#include "gui/mainwindow.h"

int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	MainWindow mw;
	mw.show();
	return app.exec();
}
