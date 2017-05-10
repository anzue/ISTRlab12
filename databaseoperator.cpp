#include "databaseoperator.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>

#include<QDate>

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
    for(int i=l;i<r;++i){
        QSqlTableModel *a = new QSqlTableModel (NULL,database);
        a->setTable(tables[i]);
        a->select();

        QTableView* x= new QTableView(NULL);
        x->setModel(a);
        x->setObjectName(tables[i]);
        //x->set
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
    QSqlQuery q(s,database);
    cout<<s.toStdString()<<" "<<affectedTableID<<'\n';

    bool b = q.exec();
    cout<<q.executedQuery().toStdString()<<' '<<b<<'\n';
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
        cout<< "Updating table version "<<tables[i].toStdString()<<'\n';
        QSqlQuery tq( "select count(*) from `"+tables[i]+"`" ,database);

        int a;
        if(tq.exec() && tq.next())
            a = tq.value(0).toInt();
        else
            a = -1;

        cout<< tq.executedQuery().toStdString()<<' ' ;
        cout<<a<<'\n';

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

    }
    return b;
}

QSqlQueryModel*  DatabaseOperator::composeQuery(QString s){
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
    auto a = database.exec("select tableID,date,size from `VersionControl`");


    res.push_back(vector<int>(tables,0));
    int i=0;
    QDate cd,nd;
    cout<<a.executedQuery().toStdString()<<' '<<a.size()<<'\n';
    while(a.next()){
        nd = a.value(1).toDate();
        //nd = QDate::fromString(c[1]);

        if(nd == cd){
            res[i][a.value(0).toInt()] = a.value(2).toInt();
        }
        else {
            cd = nd;
            res.push_back(vector<int>(tables,0));
            ++i;
            for(int j=0;j<tables;++j)
                    res[i][j] = res[i-1][j];
            res[i][a.value("tableID").toInt()] = a.value("size").toInt();
        }



        //++i;
    }

    return res;
}
