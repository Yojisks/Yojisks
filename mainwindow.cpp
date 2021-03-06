#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <iostream>
#include "administrator.h"
#include <chrono>
#include <thread>

int getResponce(QString query);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  checkTXTfile();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::checkTXTfile(){
  QString fname = "login.txt";
  QFile file(fname);
    if (QFile::exists(fname) ) {
//      hide();
//      Administrator *f1 = new Administrator;
//      f1->showMaximized();
//      f1->show();
//      on_pushButtonAutorization_clicked();
    }
}

void MainWindow::on_pushButtonAutorization_clicked()
{
  QString fname = "login.txt";
  QFile file(fname);
  if ( !QFile::exists(fname) ) {
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
      QString pwd = "login="+ui->textEditLogin->toPlainText()+"\npassword="+ui->textEditPWD->toPlainText();
      QTextStream stream(&file);
      stream << pwd;
      file.close();
    }
  }
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
      int size=query->size();
      db.close();
   //   db.removeDatabase("Users");
      return size;
    }
  }
  db.close();
  db.removeDatabase("Users");
  return 0;
}

