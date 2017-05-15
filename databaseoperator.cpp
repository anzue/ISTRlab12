#include "databaseoperator.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlRecord>
#include  <QSqlRelationalTableModel>
#include <QSqlField>
#include<QDate>
#include <QSqlError>

#include <cstring>

#include <iostream>
using namespace std;

DatabaseOperator::DatabaseOperator()
{

}

bool DatabaseOperator::OpenDatabase(
        QString type,
        QString connectionName,
        QString databaseName,
        QString userName,
        QString password){

    if(database.isOpen())
        database.close();

    database = QSqlDatabase::addDatabase(type, connectionName);
    database.setDatabaseName(databaseName);
    database.setUserName(userName);
    database.setPassword(password);
    return database.open();
}

bool DatabaseOperator::CloseDatabase(){
    if(database.isOpen()){
        database.close();
        QSqlDatabase::removeDatabase(QSqlDatabase::connectionNames()[0]);
        return 1;
    }
    else
        return 0;
}

vector<QTableView*> DatabaseOperator::GetTables(int index){
    vector<QTableView*> result;
    if(!database.open())
        return result;

    QStringList tables = database.tables();
    QSqlQuery query(database);
    int l = 0;
    int r = tables.size();
    if(index!=-1){
        l = index;
        r = index+1;
    }
/*
    QSqlTableModel *a = new QSqlTableModel (NULL,database);
    a->setTable(tables[1]);
    a->select();

    QTableView* x= new QTableView(NULL);
    x->setModel(a);
    x->setObjectName(tables[1]);
*/
   // x->show();

    for(int i=l;i<r;++i){
        auto a = new QSqlRelationalTableModel (NULL,database);
        a->setTable(tables[i]);
        a->setEditStrategy(QSqlTableModel::OnFieldChange);
        a->select();

        auto x= new QTableView;
        x->setModel(a);
        x->setObjectName(tables[i]);
        result.push_back(x);
    }
    return result;
}

DatabaseOperator::~DatabaseOperator(){

}

string toStr(int a){
    string r;

    if(a==0)
        return "0";

    while(a>0){
        r.push_back('0'+a%10);
        a/=10;
    }
    reverse(r.begin(),r.end());
    return r;
}

bool DatabaseOperator::execute(QString s,int affectedTableID){
    QSqlQuery q(database);
    bool b = q.exec(s);

    QStringList tables = database.tables();
    if(tables[tables.size()-1] == "VersionControl")
    {
        tables.pop_back();
    }
    if(affectedTableID>=tables.size())
        return 1;

    if(affectedTableID>=0 ){
        tables.clear();
        tables.push_back(database.tables()[affectedTableID]);
    }

    for(int i=0;i<tables.size();++i){
        QSqlQuery tq( "select count(*) from `"+tables[i]+"`" ,database);
        int a;
        if(tq.exec() && tq.next())
            a = tq.value(0).toInt();
        else
            a = -1;

        QSqlQuery tr( "delete  from `VersionControl` where tableName = \""+
                      tables[i] + "\" and date = \""+
                      QDate::currentDate().toString(Qt::ISODate)+"\""
                      ,database);

        tr.exec();
        QSqlQuery maxID("select max(idVC) from `VersionControl`",database);
        maxID.exec();maxID.next();
        int ID = maxID.value(0).toInt()+1;

        QSqlQuery in( "insert into VersionControl values("+
                      QString::fromStdString(toStr(ID)) +","+
                      QString::fromStdString(toStr(max(i,affectedTableID)))+",\""+
                      tables[i]+"\",\""+
                      QDate::currentDate().toString(Qt::ISODate)
                      +"\","+
                      QString::fromStdString(toStr(a))+") ",database);

            bool ff = in.exec();
            Q_UNUSED(ff);
    }
    return b;
}

QSqlQueryModel*  DatabaseOperator::composeQuery(QString s){
    if(s.toLower().toStdString().substr(0,6)!="select")
        return new QSqlQueryModel;
    QSqlQueryModel* model = new QSqlQueryModel;
    model->setQuery(s,database);
    return model;
}

vector<QString> DatabaseOperator::GetDatesList(){

    QSqlQuery q ("select distinct date from VersionControl",database);

    q.exec();
    vector<QString> d;
    while(q.next()){
        d.push_back(q.value(0).toDate().toString(Qt::LocalDate));
    }
    sort(d.begin(),d.end());
    return d;
}

vector<vector<int> > DatabaseOperator::GetStory(int tables){
    vector<vector<int> > res;

    if(!database.isOpen())
        return vector<vector<int> > ();
    auto a = database.exec("select tableName,date,size from `VersionControl`");


    res.push_back(vector<int>(tables,0));
    int i=0;
    QDate cd,nd;
    cout<<a.executedQuery().toStdString()<<' '<<a.size()<<'\n';
    while(a.next()){
        nd = a.value(1).toDate();
        //nd = QDate::fromString(c[1]);

        int ID = -1;
        QString f = a.value(0).toString();
        for(int i=0;i<database.tables().size();++i)
            if(database.tables()[i] == f)
                ID = i;

        if(nd == cd){
            res[i][ID] = a.value(2).toInt();
        }
        else {
            cd = nd;
            res.push_back(vector<int>(tables,0));
            ++i;
            for(int j=0;j<tables;++j)
                    res[i][j] = res[i-1][j];
            res[i][ID] = a.value("size").toInt();
        }



        //++i;
    }

    return res;
}

QStringList DatabaseOperator::getQuery(QString q){
    QSqlQuery tq( q ,database);
    tq.exec();

    QStringList r;
    while(tq.next()){
        r.append( tq.value(0).toString());
    }
    return r;
}

QVector<QSqlField> DatabaseOperator::getColumnList(QString table){
    QSqlRecord rec = database.record(table);
    QVector<QSqlField> r;
    //QSqlDriver dr = database.driver();

    int c = rec.count();



    for(int i=0;i<c;++i)
        r.push_back(rec.field(i));

    return r;
};
