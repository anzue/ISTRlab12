#include "mainform.h"
#include "ui_mainform.h"

#include <QTableView>

#include <QTableWidget>

#include <cstring>
#include <string>
#include <iostream>
using namespace std;

#include <QMessageBox>

#define TAB2

//#include <QtCharts>

#ifdef TAB2

#define SpecialTabsCount 2

#else

#define SpecialTabsCount 1

#endif

MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

   // ui->execute->setVisible(0);

    ui->connectionType->addItem("QMYSQL");
    ui->connectionType->addItem("QSQLITE");
    ui->connectionName->addItem("mydbConnection");

    ui->databaseName->setText("mydb");
    ui->username->setText("root");
    ui->password->setText("1111");


    ui->confirmB->hide();
    ui->valueLine->hide();

    ui->insertB->hide();
    ui->deleteB->hide();

    ui->tabWidget->setTabText(0,"Connection");

#ifdef TAB2
    ui->tabWidget->setTabText(1,"Query tab");
#else
    ui->tabWidget->removeTab(1);
#endif

  //  ui->queryTab->hide();
  //  ui->queryTab->setVisible(0);

  //  ui->infoline->setTextFormat(Qt::red);
}

MainForm::~MainForm()
{
    DeleteTabs();
    delete ui;
}

void MainForm::DeleteTabs(){
    for(int i = ui->tabWidget->count()-1;i>=2;--i){
        ui->tabWidget->removeTab(i);
    }

    for(int i=0;i<(int)tabs.size();++i){
        delete tabs[i]->model();
        delete tabs[i];
    }
    tabs.clear();
}

void MainForm::on_connectButton_clicked()
{

    if(ui->connectButton->text() == "Connect"){
        if(dbOperator.IsOpen()){
            dbOperator.CloseDatabase();
            if(ui->queryTable->model()!= 0)
            {
                delete ui->queryTable->model();
                ui->queryTable->setModel(0);
            }
            DeleteTabs();
        }
        if(!dbOperator.OpenDatabase(
                    ui->connectionType->currentText(),
                    ui->connectionName->currentText(),
                    ui->databaseName->text(),
                    ui->username->text(),
                    ui->password->text()
                    ))
        {
            ui->execute->setVisible(0);
            ui->infoline->setText("Error while connecting to database");
        }
        else {

            ui->infoline->setText("Connected");
            tabs = dbOperator.GetTables();
           // ui->tabWidget->addTab(tabs[0],tabs[0]->objectName()+ "aaaa");
            for(int i=0;i<(int)tabs.size();++i){
                ui->tabWidget->addTab(tabs[i],tabs[i]->objectName());
            }

            LaunchCustomPlotTest();
           // ui->tabWidget->

        }
        ui->connectButton->setText("Disconnect");
        return;
    }
    //delete ui->customPlot;
    //ui->customPlot = new QCustomPlot();
    //ui->customPlot->show();

//    for(int i=dbOperator.)
  //


    DeleteTabs();
    ui->queryTab->hide();
    ui->execute->setVisible(0);
    if(dbOperator.CloseDatabase())
        ui->infoline->setText("Disconnected");
    else
        ui->infoline->setText("Error while disconnecting");
    ui->connectButton->setText("Connect");
}

void MainForm::on_execute_clicked()
{
    if(ui->queryTable->model()!=NULL)
        delete ui->queryTable->model();

    QSqlQueryModel *model  = dbOperator.composeQuery(ui->query->toPlainText());
    ui->queryTable->setModel(model);
}

void MainForm::on_tabWidget_currentChanged(int index)
{
    if(index <SpecialTabsCount){
        ui->insertB->hide();
        ui->deleteB->hide();
        ui->confirmB->hide();
        ui->valueLine->hide();
    }
    else{
        ui->insertB->show();
        ui->deleteB->show();

       // updateTab(index);
    }
}

void MainForm::on_insertB_clicked()
{
    ui->insertB->hide();
    ui->valueLine->show();
    ui->confirmB->show();
}

void MainForm::on_confirmB_clicked()
{
    QString query = ui->valueLine->text();


    dbOperator.execute( "insert into `" +
                        ui->tabWidget->tabText(ui->tabWidget->currentIndex()) +
                        "` values (" +
                        query + ")" ,
                        ui->tabWidget->currentIndex() - SpecialTabsCount);

    int index = ui->tabWidget->currentIndex();
    updateTab(ui->tabWidget->count()-1);
    updateTab(index);


    ui->valueLine->hide();
    ui->confirmB->hide();
}

void MainForm::updateTab(int index){

    if(index<SpecialTabsCount)
        return;


    delete tabs[index-SpecialTabsCount];

    cout<< " Updating "<<index<<endl;
    index-=SpecialTabsCount;
    tabs[index] = dbOperator.GetTables(index)[0];

    //ui->tabWidget->removeTab(index+SpecialTabsCount);

    ui->tabWidget->insertTab(
                index+SpecialTabsCount,
                tabs[index],
                tabs[index]->objectName());

    ui->tabWidget->setCurrentIndex(index+SpecialTabsCount);
}

void MainForm::on_deleteB_clicked()
{
    //selected tab
    //int index = ui->tabWidget->currentIndex();

     QTableView* view = (QTableView*)ui->tabWidget->currentWidget();

    QItemSelectionModel *select = view->selectionModel();

    auto q = select->selectedRows();

    QAbstractItemModel * qm = (QAbstractItemModel*)view->model();

    //QString query = ui->valueLine->text();


    cout<< qm->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString().toStdString()<<'\n';
    cout<<q.size()<<'\n';
    for(int i=0;i<q.size();++i){

        dbOperator.execute( "delete from " +
                            ui->tabWidget->tabText(ui->tabWidget->currentIndex()) +
                            " where " +
                            qm->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString() + "=" +
                            qm->data(qm->index(q[i].row(),0), Qt::DisplayRole).toString(),
                            ui->tabWidget->currentIndex()-SpecialTabsCount
                            );
    }

    int index = ui->tabWidget->currentIndex();
    updateTab(ui->tabWidget->count()-1);
    updateTab(index);
}

void MainForm::on_actionAbout_triggered()
{
    QMessageBox *box  = new QMessageBox();
    box->setText("Anton Zuev\n2017\nSimple SQL Manager 6.1 ©");
    box->setWindowTitle("About");
    box->show();
}

void MainForm::LaunchCustomPlotTest(){

    auto customPlot = ui->customPlot;

    customPlot->clearGraphs();

    // set locale to english, so we get english month names:

    customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    srand(8);
    auto tables = dbOperator.GetTables();
    if(tables.size()<=0)
        return;
    if(tables[tables.size()-1]->objectName() == "VersionControl")
        tables.pop_back();
    auto story = dbOperator.GetStory(tables.size());
    auto dates = dbOperator.GetDatesList();

    if(story.size() <= 0)
        return;


    srand(time(0));
    for (int i=0; i<tables.size(); ++i)
    {
      customPlot->addGraph();
      QColor color(256*i/tables.size(),256*(tables.size()-i)/tables.size(), 150, 150);
      customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      customPlot->graph()->setPen(QPen(QBrush(color),3));
     // customPlot->graph()->setBrush(QBrush(color));
      // generate random walk data:
      QVector<QCPGraphData> data(story.size());
      for (int j=0;j<story.size(); ++j)
      {
         // cout<<story[j][i]<<' ';
        data[j].key = j;
        data[j].value = story[j][i];

        }
    //  cout<<'\n';
      customPlot->graph()->data()->set(data);

    }
    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
  //  ticker->setDateTimeFormat("d. dd MMMM\nyyyy");
    ticker->addTick(0,"0");
    for(int i=0;i<dates.size();++i)
        ticker->addTick(i+1,QString::fromStdString(dates[i].toStdString().substr(0,5)));
    customPlot->xAxis->setTicker(ticker);
    // configure left axis text labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    for(int i=0;i<tables.size();++i)
        customPlot->graph(i)->setName(tables[i]->objectName());
    for(int i=0;i<=10;++i)
        textTicker->addTick(i,QString::fromStdString(toStr(i)));


    customPlot->yAxis->setTicker(textTicker);



    // set a more compact font size for bottom and left axis tick labels:
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    // set axis labels:
    customPlot->xAxis->setLabel("Date");
    customPlot->yAxis->setLabel("Number of records");
    // make top and right axes visible but without ticks and labels:
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->xAxis2->setTicks(false);
    customPlot->yAxis2->setTicks(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setTickLabels(false);
    // set axis ranges to show all data:
    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(0, 10);
    // show legend with slightly transparent background brush:
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255, 255, 255, 150));



    ui->customPlot->replot();

}
