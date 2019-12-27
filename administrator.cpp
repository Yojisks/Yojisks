#include "administrator.h"
#include "ui_administrator.h"
#include <QTabWidget>
#include <QDataWidgetMapper>
#include <QtSql/QtSql>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QCalendarWidget>
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QDate>
#include <vector>

QSqlDatabase getConnection();

std::vector<int> ids;
Administrator::Administrator(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Administrator)
{
  ui->setupUi(this);
  ui->tabWidget->resize(Administrator::width(),Administrator::height());
  ui->calendarWidget->setSelectedDate(QDate::currentDate());
  setEnabled(false);
  printTable();
  printRequest();
}

Administrator::~Administrator()
{
  delete ui;
}

void Administrator::printRequest(){
  QDate cd = QDate::currentDate();
  ui->textEditDate->setText(cd.toString(Qt::ISODate));
  QSqlDatabase db = getConnection();
  db.open();
  if(db.isOpen()){
    QSqlQuery *query = new QSqlQuery(db);
    query->exec("SELECT Mode FROM umfdb.Mode ORDER BY `idMode`");
    QSqlRecord rec = query->record();
    int ModeCol = rec.indexOf("Mode");
    while (query->next()) {
      ui->comboBoxMode->addItem(query->value(ModeCol).toString());
    }
    query->exec("SELECT ProgrammerName FROM umfdb.Programmer ORDER BY `idProgrammer`");
    rec = query->record();
    int ProgCol = rec.indexOf("ProgrammerName");
    while (query->next()) {
      ui->comboBoxProgrammer->addItem(query->value(ProgCol).toString());
    }
    query->exec("SELECT Software_version FROM umfdb.Software_version ORDER BY `idSoftware_version`");
    rec = query->record();
    int PO = rec.indexOf("Software_version");
    while (query->next()) {
      ui->comboBoxSoftWare->addItem(query->value(PO).toString());
    }
    query->exec("SELECT Type FROM umfdb.Type ORDER BY `idType`");
    rec = query->record();
    int TypeCol = rec.indexOf("Type");
    while (query->next()) {
      ui->comboBoxType->addItem(query->value(TypeCol).toString());
    }
  }
}

void Administrator::printTable(){
  QSqlDatabase db = getConnection();
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    if(query->exec("SELECT id, ControllerID, DATE, BoardNumber,SensorNumber,70027Number, Mode, SoftwareVersion, Programmer, Type,SDCardNumber from umfdb.Controllers ORDER BY id DESC;")){
      QStandardItemModel *model = new QStandardItemModel;
      QStandardItem *item;

      //Заголовки столбцов
      QStringList horizontalHeader;
      horizontalHeader.append("№ Контроллера");
      horizontalHeader.append("Дата");
      horizontalHeader.append("Номер платы");
      horizontalHeader.append("Номер датчика");
      horizontalHeader.append("Номер 70027");
      horizontalHeader.append("Модификация");
      horizontalHeader.append("Версия ПО");
      horizontalHeader.append("Прошивший");
      horizontalHeader.append("Тип");
      //horizontalHeader.append("Причина");
      //horizontalHeader.append("Клиент");
      //horizontalHeader.append("Место установки");
      horizontalHeader.append("Номер SD карты");

      //Заголовки строк
      QStringList verticalHeader;
      for(int i=0;i<query->size();i++){
        verticalHeader.append(QString::number(i+1));
      }
      model->setHorizontalHeaderLabels(horizontalHeader);
      model->setVerticalHeaderLabels(verticalHeader);
      ids.clear();
      ids.reserve(verticalHeader.count());
      for(int i=0;i<verticalHeader.count()+1;i++){
        ids.push_back(query->value(0).toInt());
        for(int j=0;j<horizontalHeader.count();j++){
          if (query->value(j+1).toString() == ""){
            item= new QStandardItem(QString("Нет данных"));
          } else {
            item = new QStandardItem(QString(query->value(j+1).toString()));
          }
          model->setItem(i-1, j, item);
        }
        query->next();
      }
      setEnabled(false);
      ui->tableViewControllers->setModel(model);
      ui->tableViewControllers->resizeRowsToContents();
      ui->tableViewControllers->resizeColumnsToContents();
    } else {
      ui->labelStatus->setText("Статус: Ошибка соединения");
    }
  } else {
    ui->labelStatus->setText("Статус: Ошибка соединения");
  }
}

QSqlDatabase getConnection(){
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QMYSQL", "Controllers");
  db.setHostName("192.168.0.19");
  db.setDatabaseName("umfdb");
  db.setUserName("Yojisks");
  db.setPassword("Yojisks123456!");
  db.open();
  if(db.isOpen()){
    db.close();
    return db;
  }
}

void parseCSV(){
  QFile file("/home/yojisks/94/umf70026.csv");
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << file.errorString();
  }
  QStringList wordList;
  while (!file.atEnd()) {
    QByteArray line = file.readLine();
    for (int i = 0; i<13; i++){
      if((line.split(';')[i]=="-") || (line.split(';')[i]=="?")|| (line.split(';')[i]=="")){
        wordList.append("Нет данных");
      } else {
        wordList.append(line.split(';')[i]);
      }
    }
  }
  QSqlDatabase db = getConnection();
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    for(int i=0;i<wordList.count();i++){
      QString req= "INSERT INTO `umfdb`.`Controllers`(`ControllerID`,`DATE`,`BoardNumber`,`SensorNumber`,`70027Number`,`Mode`,`SoftwareVersion`,`Programmer`,`Type`,`Reason`,`Client`,`SetupPlace`,`SDCardNumber`) VALUES ('"
          +wordList[0+i*13]+"'"+","
          +"'"+wordList[1+i*13]+"'"+","
          +"'"+wordList[2+i*13]+i*13+"'"+","
          +"'"+wordList[3+i*13]+"'"+","
          +"'"+wordList[6+i*13]+"'"+","
          +"'"+wordList[4+i*13]+"'"+","
          +"'"+wordList[5+i*13]+"'"+","
          +"'"+wordList[7+i*13]+"'"+","
          +"'"+wordList[8+i*13]+"'"+","
          +"'"+wordList[9+i*13]+"'"+","
          +"'"+wordList[10+i*13]+"'"+","
          +"'"+wordList[11+i*13]+"'"+","
          +"'"+wordList[12+i*13]+"'"+")";
      if(query->exec(req)){
      } else {
      };
    }
  }
}

void Administrator::on_pushButtonEnter_clicked()
{
  addToTableControllers();
  printTable();
}

void Administrator::addToTableControllers(){
  QSqlDatabase db = getConnection();
  db.open();
  if(db.isOpen()){
    QSqlQuery *query = new QSqlQuery(db);
    QString req= "INSERT INTO `umfdb`.`Controllers`(`ControllerID`,`DATE`,`BoardNumber`,`SensorNumber`,`70027Number`,`Mode`,`SoftwareVersion`,`Programmer`,`Type`,`Reason`,`Client`,`SetupPlace`,`SDCardNumber`) VALUES ('"
        +ui->textEditControllerID->toPlainText()+"'"+","
        +"'"+ui->textEditDate->toPlainText()+"'"+","
        +"'"+ui->textEditBoardNumber->toPlainText()+"'"+","
        +"'"+ui->textEditSensorNumber->toPlainText()+"'"+","
        +"'"+ui->textEdit70027Number->toPlainText()+"'"+","
        +"'"+ui->comboBoxMode->currentText()+"'"+","
        +"'"+ui->comboBoxSoftWare->currentText()+"'"+","
        +"'"+ui->comboBoxProgrammer->currentText()+"'"+","
        +"'"+ui->comboBoxType->currentText()+"'"+","
        +"'Нет данных',"+"'Нет данных',"+"'Нет данных',"
        +"'"+ui->textEditSDCardNumber->toPlainText()+"'"+");";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
    } else {
      ui->labelStatus->setText("Статус: Ошибка");
    }
  };
}

void Administrator::on_tableViewControllers_clicked(const QModelIndex &index)
{
  ModelIndex[0] = index.row();
  ModelIndex[1] = index.column();
  QModelIndex myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 0, QModelIndex());
  ui->textEditControllerID->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 1, QModelIndex());
  ui->textEditDate->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 2, QModelIndex());
  ui->textEditBoardNumber->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 3, QModelIndex());
  ui->textEditSensorNumber->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 4, QModelIndex());
  ui->textEdit70027Number->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 5, QModelIndex());
  ui->comboBoxMode->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 6, QModelIndex());
  ui->comboBoxSoftWare->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 7, QModelIndex());
  ui->comboBoxProgrammer->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 8, QModelIndex());
  ui->comboBoxType->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ModelIndex[0], 9, QModelIndex());
  ui->textEditSDCardNumber->setText(myIndex.data().toString());
  setEnabled(true);
}

void Administrator::on_pushButtonRemove_clicked(){
  QSqlDatabase db = getConnection();
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    QString req = "DELETE FROM `umfdb`.`Controllers` WHERE id = '"+QString::number(ids[ModelIndex[0]+1])+"';";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
      printTable();
    } else {
      ui->labelStatus->setText("Статус: Ошибка"+query->lastError().text());
    }
  }
}

void Administrator::on_pushButtonRename_clicked()
{
  QSqlDatabase db = getConnection();
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    QString req = "UPDATE `umfdb`.`Controllers` SET `ControllerID` = '"+ui->textEditControllerID->toPlainText()+"'"+","+
        "`DATE` = '"+ui->textEditDate->toPlainText()+"'"+","+
        "`BoardNumber` = '"+ui->textEditBoardNumber->toPlainText()+"'"+","+
        "`SensorNumber` = '"+ui->textEditSensorNumber->toPlainText()+"'"+","+
        "`70027Number` = '"+ui->textEdit70027Number->toPlainText()+"'"+","+
        "`Mode` = '"+ui->comboBoxMode->currentText()+"'"+","+
        "`SoftwareVersion` = '"+ui->comboBoxSoftWare->currentText()+"'"+","+
        "`Programmer` = '"+ui->comboBoxProgrammer->currentText()+"'"+","+
        "`Type` = '"+ui->comboBoxType->currentText()+"'"+","+
//        "`Reason` = <{Reason: }>, "+
//        "`Client` = <{Client: }>, "+
//        "`SetupPlace` = <{SetupPlace: }>, "+
        "`SDCardNumber` = '"+ui->textEditSDCardNumber->toPlainText()+"'"+
        "WHERE `id` ='"+QString::number(ids[ModelIndex[0]+1])+"';";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
      printTable();
    } else {
      qDebug() << req;
      ui->labelStatus->setText("Статус: Ошибка"+query->lastError().text());
    }
  }
}

void Administrator::on_textEdit_textChanged()
{
  QSqlDatabase db = getConnection();
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
  QString req = "SELECT id, ControllerID, DATE, BoardNumber,SensorNumber,70027Number, Mode, SoftwareVersion, Programmer, Type,SDCardNumber from umfdb.Controllers WHERE `ControllerID` like '%"
                   +ui->textEdit->toPlainText()+"%' or `DATE` like '%"
                   +ui->textEdit->toPlainText()+"%' or `BoardNumber` like '%"
                   +ui->textEdit->toPlainText()+"%' or `SensorNumber` like '%"
                   +ui->textEdit->toPlainText()+"%' or `70027Number` like '%"
                   +ui->textEdit->toPlainText()+"%' or `Mode` like '%"
                   +ui->textEdit->toPlainText()+"%' or `SoftwareVersion` like '%"
                   +ui->textEdit->toPlainText()+"%' or `Programmer` like '%"
                   +ui->textEdit->toPlainText()+"%' or `Type` like '%"
                   +ui->textEdit->toPlainText()+"%' or `SDCardNumber` like '%"
                   +ui->textEdit->toPlainText()+"%' ORDER BY id DESC;";
      if(query->exec(req)){
      QStandardItemModel *model = new QStandardItemModel;
      QStandardItem *item;
      //Заголовки столбцов
      QStringList horizontalHeader;
      horizontalHeader.append("№ Контроллера");
      horizontalHeader.append("Дата");
      horizontalHeader.append("Номер платы");
      horizontalHeader.append("Номер датчика");
      horizontalHeader.append("Номер 70027");
      horizontalHeader.append("Модификация");
      horizontalHeader.append("Версия ПО");
      horizontalHeader.append("Прошивший");
      horizontalHeader.append("Тип");
      //horizontalHeader.append("Причина");
      //horizontalHeader.append("Клиент");
      //horizontalHeader.append("Место установки");
      horizontalHeader.append("Номер SD карты");

      //Заголовки строк
      QStringList verticalHeader;
      for(int i=0;i<query->size();i++){
        verticalHeader.append(QString::number(i+1));
      }
      model->setHorizontalHeaderLabels(horizontalHeader);
      model->setVerticalHeaderLabels(verticalHeader);
      ids.clear();
      ids.reserve(verticalHeader.count());
      for(int i=0;i<verticalHeader.count()+1;i++){
        ids.push_back(query->value(0).toInt());
        for(int j=0;j<horizontalHeader.count();j++){
          if (query->value(j+1).toString() == ""){
            item= new QStandardItem(QString("Нет данных"));
          } else {
            item = new QStandardItem(QString(query->value(j+1).toString()));
          }
          model->setItem(i-1, j, item);
        }
        query->next();
      }
      setEnabled(false);
      ui->tableViewControllers->setModel(model);
      ui->tableViewControllers->resizeRowsToContents();
      ui->tableViewControllers->resizeColumnsToContents();
    } else {
      ui->labelStatus->setText("Статус: Ошибка соединения");
    }
  } else {
    ui->labelStatus->setText("Статус: Ошибка соединения");
  }
}

void Administrator::setEnabled(bool enable){
  ui->pushButtonRemove->setEnabled(enable);
  ui->pushButtonRename->setEnabled(enable);
}

void Administrator::on_calendarWidget_currentPageChanged(int year, int month)
{

}

void Administrator::on_calendarWidget_selectionChanged()
{
  ui->textEditDate->setText(ui->calendarWidget->selectedDate().toString(Qt::ISODate));
}
