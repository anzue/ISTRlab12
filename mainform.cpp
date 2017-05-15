#include "mainform.h"
#include "ui_mainform.h"

#include <QTableView>

#include <QTableWidget>
#include <QVariant>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

#include <QMessageBox>

#define TAB2

#ifdef TAB2

#define SpecialTabsCount 2

#else

#define SpecialTabsCount 1

#endif

#define SQ static QString

namespace LC{

    static QString connect;// = "Connect";
    static QString disconnect;// = "Disconnect";
    static QString connected;// = "Connected";
    static QString mainTab;// = "Connection";
    static QString queryTab;// = "Query tab";
    static QString insert;
    static QString del;
    static QString help;
    static QString lang;
    static QString conname;




    static void toUkrainian(){
        lang = "Мова";
        help = "Допомога";
        connect  = "Зєдатися";
        disconnect ="Відєднатися";
        connected="Зєднано";
        mainTab="Зєднання";
        queryTab="Запит";
        insert = "Вставити";
        del = "Видалити";
    }

    static void toEnglish(){
        lang = "Language";
        help = "Help";
        connect = "Connect";
        disconnect = "Disconnect";
        connected = "Connected";
        mainTab = "Connection";
        queryTab = "Query tab";
        insert = "Insert";
        del = "Delete";
    }

    static void Init(){

        toUkrainian();

       // toEnglish();
    }
};

namespace QE{

    static QString givenRel;
    static QString noGivenRel;
    static QString containedRel;
    static QString legionsRel;
    static QString raceGroup;
    static QString raceReligions;
    static QString legionsToRel;
    static QString prName;

    static void Init(){
        legionsRel = "Знайти всі легіони,що належать до тої ж релігії,що й вибраний";
        raceGroup = "Знайти всі групи вибраної раси";
        givenRel = "Знайти всі раси та їх групи,що належать заданій релігії";
        noGivenRel = "Знайти всі раси, жодна з груп якої не належить до даної релігії";
        raceReligions = "Знайти всі релігії, прихильники яких є серед заданої раси";

        containedRel = "Знайти всі раси, що поклоняються щонайменше тим релігіям,що й задана";
        legionsToRel = "Знайти усі раси, які поклюняються лише деяким"
                       "з тих релігій,яким поклоняються легіони з номерами, "
                       "що не перевищують заданого";

        prName = "Знайти імена всіх примархів, чиї легіони бились в усіх тих битвах що й заданий легіон";
    }
}
MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    LC::Init();
    QE::Init();


    ui->connectionType->addItem("QMYSQL");
    ui->connectionType->addItem("QSQLITE");
    ui->connectionName->addItem("mydbConnection");

    ui->databaseName->setText("mydb");
    ui->username->setText("root");
    ui->password->setText("1111");

    ui->tabWidget->setTabText(0,LC::mainTab);

    ui->insertionPanel->hide();

#ifdef TAB2
    ui->tabWidget->setTabText(1,LC::queryTab);
#else
    ui->tabWidget->removeTab(1);
#endif

    ui->QueryTypeBox->addItem(QE::legionsRel);
    ui->QueryTypeBox->addItem(QE::givenRel);
    ui->QueryTypeBox->addItem(QE::noGivenRel);
    ui->QueryTypeBox->addItem(QE::raceGroup);
    ui->QueryTypeBox->addItem(QE::raceReligions);

    ui->QueryTypeBox->addItem(QE::containedRel);
    ui->QueryTypeBox->addItem(QE::legionsToRel);
    ui->QueryTypeBox->addItem(QE::prName);

    ui->Queries->hide();

    ui->Queries->setTabText(0,"Ручне введення запитів");
    ui->Queries->setTabText(1,"Інфографіка");
    ui->Queries->setTabText(2,"Автоматизовані запити");


    on_connectButton_clicked();
    on_QueryTypeBox_currentTextChanged(ui->QueryTypeBox->currentText());
}

void MainForm::on_QueryTypeBox_currentTextChanged(const QString &t)
{

    ui->parametr->clear();
    QStringList l;
    if(t == QE::givenRel || t == QE::noGivenRel){

        l = dbOperator.getQuery("select relName from `Religions`");
        }
    if(t == QE::containedRel || t == QE::raceReligions){
        l = dbOperator.getQuery("select raceName from `Races`");
    }

    if(t == QE::legionsRel || t == QE::prName){
        l = dbOperator.getQuery("select name from `Legions`");
    }
    if(t == QE::raceGroup){
        l =  dbOperator.getQuery("select raceName from `Races`");
    }
    if(t == QE::legionsToRel){
        l = dbOperator.getQuery("select legId from Legions");
    }

    ui->parametr->addItems(l);
}

void MainForm::on_dbQuery_clicked()
{
    QString q;
    QString t = ui->QueryTypeBox->currentText();
    QString p = ui->parametr->currentText();
    if(t == QE::givenRel){
        q = "select raceName,sqName from ("
            "`Races` inner join `Squard` "
                    "on Races.raceID = Squard.raceID)"
            "inner join `Religions` "
                    "on Squard.relID = Religions.relID "
            "where relName = \""+ p +"\"";

    }
    if(t == QE::noGivenRel){
        q = "select raceName from Races "
                "where not exists ( "
                "select * from "
                "(`Squard` inner join `Religions`  "
                                    "on Squard.relID = Religions.relID ) "
                "where relName = \""+ p + "\" and Squard.raceID = Races.raceID) ";
    }
    if(t == QE::containedRel){
       q =
               "select raceName from Races "
               "where not exists( "
                       "(select S.relID "
                       "from Squard as S inner join Races as R "
                           "on S.raceID=R.raceID "
                       "where R.raceName = \""+p+"\" "
                       "and S.relID not in "
                           "(select relID from Squard "
                           "where Races.raceID = Squard.raceID)))";

    }

    if(t == QE::legionsRel){
       q =  "select Legions.name "
        "from Legions inner join Squard on Legions.sqID = Squard.sqID "
        "where Squard.relID in ( "
            "select relID "
            "from (Legions as L inner join Squard as S on L.sqID=S.sqID) "
            "where L.name = \""+ p + "\" and Squard.relID = S.relID)";
    }

    if(t == QE::raceGroup){
        q = "select Squard.sqName "
                "from Squard inner join Races "
                "on Races.raceID = Squard.raceID "
                "where Races.raceName = \""+p+"\"";
    }

    if(t == QE::raceReligions){
        q = "select Religions.relName "
                "from (Religions inner join Squard "
                "on Religions.relID = Squard.relID) "
                "inner join Races on Squard.raceID = Races.raceID "
            "where raceName = \""+p+"\" ";
    }

    if(t == QE::legionsToRel){
        q = "select Races.raceName "
                "from Races "
                "where not exists( "
                "    select S.relID from Squard as S "
                "    where S.raceID = Races.raceID and S.relID not in("
                "        select distinct Squard.relID "
                "        from Squard inner join Legions"
                "        on Squard.sqID = Legions.sqId"
                "        where Legions.legID <="+p+"))";
    }

    if(t == QE::prName){
        q = "select Primarhs.prName "
            "    from Primarhs inner join Legions as L1"
            "        on Primarhs.legID = L1.legID"
            "    where "
            "        not exists( "
            "            select LegionsBattles.btID "
            "            from LegionsBattles inner join Legions as L2"
            "                on L2.legID = LegionsBattles.legID"
            "            where"
            "                L2.name = \""+p+"\" and"
            "                LegionsBattles.btID not in("
            "                    select btID from LegionsBattles as LB1"
            "                    where LB1.legID = L1.legID"
            "                )"
            "            )";
    }

    auto mod = dbOperator.composeQuery(q);
    ui->dbView->setModel(mod);

}


MainForm::~MainForm()
{
    DeleteTabs();
    delete ui;
}

void MainForm::DeleteTabs(){
    for(int i = ui->tabWidget->count()-1;i>=SpecialTabsCount;--i){
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

    if(ui->connectButton->text() == LC::connect){
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
            ui->statusBar->showMessage("Помилка при підключенні до бази данних",100000);
            return;
        }
        else {
            ui->execute->setVisible(1);
            ui->Queries->show();
            ui->statusBar->showMessage(LC::connected,10000);
            tabs = dbOperator.GetTables();

            for(int i=0;i<(int)tabs.size();++i){
                if(tabs[i]->objectName()=="VersionControl")
                    continue;
                ui->tabWidget->addTab((tabs[i]),tabs[i]->objectName());
                tabs[i]->model()->setParent(this);
            }

            LaunchCustomPlotTest();

        }
        ui->connectButton->setText(LC::disconnect);
        return;
    }

    DeleteTabs();
    //ui->queryTab->hide();
    ui->Queries->hide();
    ui->execute->setVisible(0);
    if(dbOperator.CloseDatabase())
        ui->statusBar->showMessage("Розєднано",10000);
    ui->connectButton->setText(LC::connect);


}

void MainForm::on_execute_clicked()
{
    if(ui->queryTable->model()!=NULL)
        delete ui->queryTable->model();

    QSqlQueryModel *model  = dbOperator.composeQuery(ui->query->toPlainText());
    ui->queryTable->setModel(model);
}

QString transformate(QString a){

    cout<<a.toStdString()<<'\n';

    if(a=="int")
        return "Число";
    if(a=="QString")
        return "Текст";
    return a;

}

void MainForm::on_tabWidget_currentChanged(int index)
{
    if(index <SpecialTabsCount){

        ui->insertionPanel->hide();


        for(int i=0;i<insertionFields.size();++i)
            delete insertionFields[i];


        insertionFields.clear();

        ui->insertionPanel->hide();
    }
    else{


        for(int i=0;i<insertionFields.size();++i)
            delete insertionFields[i];

        for(int i=0;i<labels.size();++i)
            delete labels[i];

        insertionFields.clear();
        labels.clear();

       ui->insertionPanel->show();


       auto l = dbOperator.getColumnList(
                   dbOperator.GetTables(
                       ui->tabWidget->currentIndex()-SpecialTabsCount  )[0]->objectName());
       // cout<<l.size()<<'\n';

        for(int i=0;i<l.size();++i){
            auto h = new QLineEdit(ui->insertionPanel);

            h->show();
            h->resize(100,30);
            h->move(15+100*i,0);

            auto la = new QLabel(ui->insertionPanel);



            la->setText(transformate(l[i].value().typeName()));
            la->setObjectName((l[i].value().typeName()));
            la->show();
            la->resize(100,30);
            la->move(15+100*i,30);
            labels.push_back(la);
            insertionFields.push_back(h);
        }

    }
}

void MainForm::on_insertB_clicked()
{
    QString q = "insert into `" +
            ui->tabWidget->tabText(ui->tabWidget->currentIndex())+
            "` values (";
    for(int i=0;i<insertionFields.size();++i)
        if(  labels[i]->objectName() == "int" || labels[i]->objectName() == "Ціле")
            q = q+ insertionFields[i]->text() + " ,";
        else
            q = q + "\""+ insertionFields[i]->text() + "\",";

    q.remove(q.length()-1,1);
    q = q + ")";
    if(!dbOperator.execute(q,ui->tabWidget->currentIndex()- SpecialTabsCount))
        ui->statusBar->showMessage("Помилка при вставці, перевірте формат данних та спробуйте ще раз",10000);
    else
        ui->statusBar->showMessage("Вставка Успішна",10000);

    int id = ui->tabWidget->currentIndex();

   // updateTab(ui->tabWidget->count()-1);
    updateTab(id);

}

void MainForm::on_confirmB_clicked(){}

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

        dbOperator.execute( "delete from `" +
                            ui->tabWidget->tabText(ui->tabWidget->currentIndex()) +
                            "` where " +
                            qm->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString() + "=" +
                            qm->data(qm->index(q[i].row(),0), Qt::DisplayRole).toString(),
                            ui->tabWidget->currentIndex()-SpecialTabsCount
                            );
    }

    int index = ui->tabWidget->currentIndex();
    updateTab(index);
}

void MainForm::on_actionAbout_triggered()
{
    QMessageBox *box  = new QMessageBox();
    box->setText("Anton Zuev\n2017\nSimple SQL Manager 6.1 ©");
    box->setWindowTitle("About");
    box->show();
}



void MainForm::renewTab(){

    ui->tabWidget->setTabText(0,LC::mainTab);
    ui->tabWidget->setTabText(1,LC::queryTab);

    if(!dbOperator.IsOpen())
        ui->connectButton->setText(LC::connect);
    else
        ui->connectButton->setText(LC::disconnect);
    ui->insertB->setText(LC::insert);
    ui->deleteB->setText(LC::del);

}

void MainForm::on_actionUkrainian_triggered()
{
    LC::toUkrainian();
    renewTab();
}



void MainForm::on_actionEnglish_triggered()
{
    LC::toEnglish();
    renewTab();
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
    for (int i=0; i<(int)tables.size(); ++i)
    {
      customPlot->addGraph();
      QColor color(256*i/tables.size(),256*(tables.size()-i)/tables.size(), 150, 150);
      customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      customPlot->graph()->setPen(QPen(QBrush(color),3));

      QVector<QCPGraphData> data(story.size());
      for (int j=0;j<(int)story.size(); ++j)
      {
        data[j].key = j;
        data[j].value = story[j][i];
        }
      customPlot->graph()->data()->set(data);

    }
    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
  //  ticker->setDateTimeFormat("d. dd MMMM\nyyyy");
    ticker->addTick(0,"05.05");
    for(int i=0;i<(int)dates.size();++i)
        ticker->addTick(i+1,QString::fromStdString(dates[i].toStdString().substr(0,5)));
    customPlot->xAxis->setTicker(ticker);
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for(int i=0;i<(int)tables.size();++i)
        customPlot->graph(i)->setName(tables[i]->objectName());
    int Max = dbOperator.getQuery("select max(size) from VersionControl")[0].toInt()+3;
    for(int i=0;i<=Max;++i)
        textTicker->addTick(i,QString::fromStdString(toStr(i)));

    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->xAxis->setLabel("Date");
    customPlot->yAxis->setLabel("Number of records");
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->xAxis2->setTicks(false);
    customPlot->yAxis2->setTicks(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setTickLabels(false);
    customPlot->xAxis->setRange(0, dates.size()+5);
    customPlot->yAxis->setRange(0, Max);
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255, 255, 255, 150));

    ui->customPlot->replot();
}
