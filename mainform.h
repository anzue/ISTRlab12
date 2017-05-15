#ifndef MAINFORM_H
#define MAINFORM_H

#include <QMainWindow>
#include <QLabel>

#include <databaseoperator.h>

namespace Ui {
class MainForm;
}
#include<vector>
using std::vector;



class MainForm : public QMainWindow
{
    Q_OBJECT
    DatabaseOperator dbOperator;
    vector<QTableView*> tabs;

    vector<QLineEdit *> insertionFields;
    vector<QLabel*> labels;
  //  vector<QSqlTableModel*>models;

public:

    void LaunchCustomPlotTest();

    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private slots:
    void on_connectButton_clicked();

    void on_execute_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_insertB_clicked();

    void on_confirmB_clicked();

    void on_deleteB_clicked();

    //void on_deleteB_clicked();
    
    void on_actionAbout_triggered();

    void on_actionUkrainian_triggered();

    void renewTab();

    void on_actionEnglish_triggered();

    void on_QueryTypeBox_currentTextChanged(const QString &arg1);

    void on_dbQuery_clicked();

private:
    Ui::MainForm *ui;

    void DeleteTabs();
    void updateTab(int index);


};

#endif // MAINFORM_H
