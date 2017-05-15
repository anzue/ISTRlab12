#ifndef DATABASEOPERATOR_H
#define DATABASEOPERATOR_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <string>
#include <QSqlQueryModel>
#include<QVector>
#include<QSqlField>
#include<QTableWidget>

#include<QTableView>

#include <vector>
using std::vector;

class DatabaseOperator
{
     QSqlDatabase database;
public:
    DatabaseOperator();
    ~DatabaseOperator();

    bool IsOpen(){return database.isOpen();}

    bool OpenDatabase( QString type,
                       QString connectionName,
                       QString databaseName,
                       QString userName,
                       QString password);

    bool CloseDatabase();

    vector<QTableView*> GetTables(int index = -1);

    bool execute(QString,int affectedTableID = -1);

    QSqlQueryModel*  composeQuery(QString);

    vector<QString> GetDatesList();
    vector<vector<int> > GetStory(int tables = 0);

    QStringList getQuery(QString);

    QVector<QSqlField> getColumnList(QString);
};

std::string toStr(int a);


#endif // DATABASEOPERATOR_H
