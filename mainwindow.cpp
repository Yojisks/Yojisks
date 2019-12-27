#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <iostream>
#include "administrator.h"

int getResponce(QString query);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButtonAutorization_clicked()
{
  QString autorization;
  autorization = "SELECT Permissions FROM umfdb.Users where User = '"+ui->textEditLogin->toPlainText()+"' and Password = '"+ui->textEditPWD->toPlainText()+"'";
  if(getResponce(autorization)){
    hide();
    Administrator *f1 = new Administrator;
    f1->showMaximized();
    f1->show();
  }
}


int getResponce(QString request){
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QMYSQL", "Users");
  db.setHostName("192.168.0.19");
  db.setDatabaseName("umfdb");
  db.setUserName("Yojisks");
  db.setPassword("Yojisks123456!");
  db.open();
  if(db.isOpen()){
    QSqlQuery *query = new QSqlQuery(db);
    if(query->exec(request)){
      return query->size();
      db.close();
    }
  }
  db.close();
  return 0;
}

