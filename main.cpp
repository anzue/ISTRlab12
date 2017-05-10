#include "mainform.h"
#include <QApplication>


#include <QtSql/QSqlDatabase>
#include <QtSql/QtSql>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainForm w;
    w.show();
    w.LaunchCustomPlotTest();

    return a.exec();
}
