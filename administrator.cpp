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
#include <QString>
#include <vector>

QSqlDatabase getConnection(QString ConnectionName);
bool checkString(QString str);

std::vector<int> ids;
std::vector<int> idUmfs;
Administrator::Administrator(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Administrator)
{
  ui->setupUi(this);
  ui->tabWidget->resize(Administrator::width(),Administrator::height());
  ui->calendarWidget->setSelectedDate(QDate::currentDate());
  setEnabledControllerButtons(false);
  setEnabledPlateButtons(false);
  ui->tableViewControllers->setSortingEnabled(true);
  printTableControllers();
  printTablePlates();
  printRequest();
}

Administrator::~Administrator()
{
  delete ui;
}

void Administrator::printRequest(){
  QDate cd = QDate::currentDate();
  ui->textEditDate->setText(cd.toString(Qt::ISODate));
  QSqlDatabase db = getConnection("Controllers");
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
      ui->comboBoxUser->addItem(query->value(ProgCol).toString());
    }
    query->exec("SELECT Software_version FROM umfdb.Software_version ORDER BY `idSoftware_version` DESC LIMIT 10");
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

void Administrator::printTableControllers(){
  QSqlDatabase db = getConnection("Controllers");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    if(query->exec("SELECT *"/* id, ControllerID, DATE, BoardNumber,SensorNumber,70027Number, Mode, SoftwareVersion, Programmer, Type,SDCardNumber*/ "from umfdb.Controllers ORDER BY id DESC;")){
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
      horizontalHeader.append("Причина");
      horizontalHeader.append("Клиент");
      horizontalHeader.append("Место установки");
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
      setEnabledControllerButtons(false);
      ui->tableViewControllers->setModel(model);
      ui->tableViewControllers->resizeRowsToContents();
      ui->tableViewControllers->resizeColumnsToContents();
      ui->tableViewControllers->setColumnHidden(9,true);
      ui->tableViewControllers->setColumnHidden(10,true);
      ui->tableViewControllers->setColumnHidden(11,true);
    } else {
      ui->labelStatus->setText("Статус: Ошибка соединения");
    }
  } else {
    ui->labelStatus->setText("Статус: Ошибка соединения");
  }
}

void Administrator::printTablePlates(){
  QSqlDatabase db = getConnection("Plates");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    if(query->exec("SELECT * from umfdb.Umfs ORDER BY idUmf DESC;")){
      QStandardItemModel *model = new QStandardItemModel;
      QStandardItem *item;

      //Заголовки столбцов
      QStringList horizontalHeader;
      horizontalHeader.append("№ Платы");
      horizontalHeader.append("Дата");
      horizontalHeader.append("Версия ПО");
      horizontalHeader.append("Имя ПО");
      horizontalHeader.append("Сотрудник");
      horizontalHeader.append("Настройка");
      horizontalHeader.append("Ошибки");
      horizontalHeader.append("Lock");
      //      horizontalHeader.append("Прошивший");
      //      horizontalHeader.append("Тип");
      //      //horizontalHeader.append("Причина");
      //      //horizontalHeader.append("Клиент");
      //      //horizontalHeader.append("Место установки");
      //      horizontalHeader.append("Номер SD карты");
      //Заголовки строк
      QStringList verticalHeader;
      for(int i=0;i<query->size();i++){
        verticalHeader.append(QString::number(i+1));
      }
      model->setHorizontalHeaderLabels(horizontalHeader);
      model->setVerticalHeaderLabels(verticalHeader);
      idUmfs.clear();
      idUmfs.reserve(verticalHeader.count());
      for(int i=0;i<verticalHeader.count()+1;i++){
        idUmfs.push_back(query->value(0).toInt());
        if (query->value(8).toString() == "red"){
          red[i]=1;
        } else {
          red[i]=0;
        }
        for(int j=0;j<horizontalHeader.count();j++){
          if (query->value(j).toString() == ""){
            item= new QStandardItem(QString("Нет данных"));
          } else {
            item = new QStandardItem(QString(query->value(j).toString()));
          }
          model->setItem(i-1, j, item);
        }
        query->next();
      }
      setEnabledControllerButtons(false);
      ui->tableViewPlates->setModel(model);
      ui->tableViewPlates->resizeRowsToContents();
      ui->tableViewPlates->resizeColumnsToContents();
      for(int j=0;j<verticalHeader.count();j++){
        if(red[j]==1){
          for(int i(0); i < ui->tableViewPlates->model()->columnCount(); i++)
          {
            ui->tableViewPlates->model()->setData(ui->tableViewPlates->model()->index(j-1, i), QColor(Qt::red), Qt::BackgroundRole);
          }
        }
      }
    } else {
      ui->labelStatus->setText("Статус: Ошибка соединения");
    }
  } else {
    ui->labelStatus->setText("Статус: Ошибка соединения");
  }
  ui->tableViewPlates->setColumnWidth(2,250);
  ui->tableViewPlates->setColumnWidth(3,100);
  ui->tableViewPlates->setColumnWidth(0,80);
  ui->tableViewPlates->setColumnWidth(1,80);
}

QSqlDatabase getConnection(QString ConnectionName){
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QMYSQL", ConnectionName);
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
  QSqlDatabase db = getConnection("Controllers");
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
  printTableControllers();
}

void Administrator::addToTableControllers(){
  QSqlDatabase db = getConnection("Controllers");
  db.open();
  if(db.isOpen()){
    QSqlQuery *query = new QSqlQuery(db);
    //if((checkString(ui->textEditControllerID->toPlainText())) || (checkString(ui->textEditDate->toPlainText())) || (checkString(ui->textEditBoardNumber->toPlainText())) || (checkString(ui->textEditSensorNumber->toPlainText())))
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
        +"'"+ui->textEditReason->toPlainText()+"'"+","
        +"'"+ui->textEditClient->toPlainText()+"'"+","
        +"'"+ui->textEditSetupPlace->toPlainText()+"'"+","
        +"'"+ui->textEditSDCardNumber->toPlainText()+"'"+");";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
    } else {
      ui->labelStatus->setText("Статус: Ошибка");
    }
  };
}


void Administrator::addToTablePlates(){
  QSqlDatabase db = getConnection("Plates");
  db.open();
  if(db.isOpen()){
    QSqlQuery *query = new QSqlQuery(db);
    //if((checkString(ui->textEditControllerID->toPlainText())) || (checkString(ui->textEditDate->toPlainText())) || (checkString(ui->textEditBoardNumber->toPlainText())) || (checkString(ui->textEditSensorNumber->toPlainText())))
    QString req= "INSERT INTO `umfdb`.`Umfs` VALUES ('"
        +ui->textPlateNumber->toPlainText()+"'"+","
        +"'"+ui->textPlateEditDate->toPlainText()+"'"+","
        +"'"+ui->textPlateVersion->toPlainText()+"'"+","
        +"'"+ui->textPlateName->toPlainText()+"'"+","
        +"'"+ui->comboBoxUser->currentText()+"'"+","
        +"'"+ui->textPlateSettings->toPlainText()+"'"+","
        +"'"+ui->textPlateErrors->toPlainText()+"'"+","
        +"'"+ui->comboBoxLock->currentText()+"'"+");";
    if(query->exec(req)){
      ui->labelPlateStatus->setText("Статус: Всё хорошо");
    } else {
      ui->labelPlateStatus->setText("Статус: Ошибка");
    }
  };
}

void Administrator::on_tableViewControllers_clicked(const QModelIndex &index)
{
  ControllerModelIndex[0] = index.row();
  ControllerModelIndex[1] = index.column();
  QModelIndex myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 0, QModelIndex());
  ui->textEditControllerID->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 1, QModelIndex());
  ui->textEditDate->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 2, QModelIndex());
  ui->textEditBoardNumber->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 3, QModelIndex());
  ui->textEditSensorNumber->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 4, QModelIndex());
  ui->textEdit70027Number->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 5, QModelIndex());
  ui->comboBoxMode->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 6, QModelIndex());
  ui->comboBoxSoftWare->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 7, QModelIndex());
  ui->comboBoxProgrammer->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 8, QModelIndex());
  ui->comboBoxType->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 9, QModelIndex());
  ui->textEditReason->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 10, QModelIndex());
  ui->textEditClient->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 11, QModelIndex());
  ui->textEditSetupPlace->setText(myIndex.data().toString());
  myIndex = ui->tableViewControllers->model()->index( ControllerModelIndex[0], 12, QModelIndex());
  ui->textEditSDCardNumber->setText(myIndex.data().toString());
  setEnabledControllerButtons(true);//TODO: Может здесь, может не здесь придумать сортировку, пока на ум ничего не пришло
}

void Administrator::on_pushButtonRemove_clicked(){
  QSqlDatabase db = getConnection("Controllers");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    QString req = "DELETE FROM `umfdb`.`Controllers` WHERE id = '"+QString::number(ids[ControllerModelIndex[0]+1])+"';";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
      printTableControllers();
    } else {
      ui->labelStatus->setText("Статус: Ошибка"+query->lastError().text());
    }
  }
}

void Administrator::on_pushButtonRename_clicked()
{
  QSqlDatabase db = getConnection("Controllers");
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
        "`Reason` = '"+ui->textEditReason->toPlainText()+"'"+","+
        "`Client` = '"+ui->textEditClient->toPlainText()+"'"+","+
        "`SetupPlace` = '"+ui->textEditSetupPlace->toPlainText()+"'"+","+
        "`SDCardNumber` = '"+ui->textEditSDCardNumber->toPlainText()+"'"+
        "WHERE `id` ='"+QString::number(ids[ControllerModelIndex[0]+1])+"';";
    if(query->exec(req)){
      ui->labelStatus->setText("Статус: Всё хорошо");
      printTableControllers();
    } else {
      qDebug() << req;
      ui->labelStatus->setText("Статус: Ошибка"+query->lastError().text());
    }
  }
}

void Administrator::on_textEdit_textChanged()
{
  QSqlDatabase db = getConnection("Controllers");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if((db.isOpen()) && (checkString(ui->textEdit->toPlainText()))){
    QString req = "SELECT *"/*id, ControllerID, DATE, BoardNumber,SensorNumber,70027Number, Mode, SoftwareVersion, Programmer, Type,SDCardNumber*/" from umfdb.Controllers WHERE `ControllerID` like '%"
        +ui->textEdit->toPlainText()+"%' or `DATE` like '%"
        +ui->textEdit->toPlainText()+"%' or `BoardNumber` like '%"
        +ui->textEdit->toPlainText()+"%' or `SensorNumber` like '%"
        +ui->textEdit->toPlainText()+"%' or `70027Number` like '%"
        +ui->textEdit->toPlainText()+"%' or `Mode` like '%"
        +ui->textEdit->toPlainText()+"%' or `SoftwareVersion` like '%"
        +ui->textEdit->toPlainText()+"%' or `Programmer` like '%"
        +ui->textEdit->toPlainText()+"%' or `Type` like '%"
        +ui->textEdit->toPlainText()+"%' or `SDCardNumber` like '%"
        +ui->textEdit->toPlainText()+"%' or `Reason` like '%"
        +ui->textEdit->toPlainText()+"%' or `Client` like '%"
        +ui->textEdit->toPlainText()+"%' or `SetupPlace` like '%"
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
      horizontalHeader.append("Причина");
      horizontalHeader.append("Клиент");
      horizontalHeader.append("Место установки");
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
      setEnabledControllerButtons(false);
      ui->tableViewControllers->setModel(model);
      ui->tableViewControllers->resizeRowsToContents();
      ui->tableViewControllers->resizeColumnsToContents();
      ui->tableViewControllers->setColumnHidden(9,true);
      ui->tableViewControllers->setColumnHidden(10,true);
      ui->tableViewControllers->setColumnHidden(11,true);
    } else {
      ui->labelStatus->setText("Статус: Ошибка соединения");
    }
  } else {
    ui->labelStatus->setText("Статус: Ошибка соединения");
  }
}

void Administrator::setEnabledControllerButtons(bool enable){
  ui->pushButtonRemove->setEnabled(enable);
  ui->pushButtonRename->setEnabled(enable);
}

void Administrator::setEnabledPlateButtons(bool enable){
  ui->pushButtonMove->setEnabled(enable);
  ui->pushButtonPlateRemove->setEnabled(enable);
}

bool checkString(QString str){
  if((str.contains("%")) || (str.contains(";")) || (str.contains("'")) || (str.contains("+"))){
    return false;
  }
  return true;
}

void Administrator::on_calendarWidget_clicked(const QDate &date)
{
  ui->textEditDate->setText(date.toString(Qt::ISODate));
}

void Administrator::on_pushButtonAdd_clicked()
{
  addToTablePlates();
  printTablePlates();
}

void Administrator::on_tableViewPlates_clicked(const QModelIndex &index)
{
  PlatesModelIndex[0] = index.row();
  PlatesModelIndex[1] = index.column();
  QModelIndex myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 1, QModelIndex());
  ui->textPlateEditDate->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 0, QModelIndex());
  ui->textPlateNumber->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 2, QModelIndex());
  ui->textPlateVersion->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 3, QModelIndex());
  ui->textPlateName->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 4, QModelIndex());
  ui->comboBoxUser->setCurrentText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 5, QModelIndex());
  ui->textPlateSettings->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 6, QModelIndex());
  ui->textPlateErrors->setText(myIndex.data().toString());
  myIndex = ui->tableViewPlates->model()->index( PlatesModelIndex[0], 7, QModelIndex());
  ui->comboBoxLock->setCurrentText(myIndex.data().toString());
  setEnabledPlateButtons(true);//TODO: Может здесь, может не здесь придумать сортировку, пока на ум ничего не пришло
}

void Administrator::on_textEditPlateSearch_textChanged()
{
  QSqlDatabase db = getConnection("Plates");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if((db.isOpen()) && (checkString(ui->textEdit->toPlainText()))){
    if(query->exec("SELECT * from umfdb.Umfs WHERE `idUmf` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `Date` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `SoftwareVersion` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `SoftwareName` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `User` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `Settings` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `Errors` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' or `Lock` like '%"
                   +ui->textEditPlateSearch->toPlainText()+"%' ORDER BY idUmf DESC;")){
      QStandardItemModel *model = new QStandardItemModel;
      QStandardItem *item;
      //Заголовки столбцов
      QStringList horizontalHeader;
      horizontalHeader.append("№ Платы");
      horizontalHeader.append("Дата");
      horizontalHeader.append("Версия ПО");
      horizontalHeader.append("Имя ПО");
      horizontalHeader.append("Сотрудник");
      horizontalHeader.append("Настройка");
      horizontalHeader.append("Ошибки");
      horizontalHeader.append("Lock");
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
          if (query->value(j).toString() == ""){
            item= new QStandardItem(QString("Нет данных"));
          } else {
            item = new QStandardItem(QString(query->value(j).toString()));
          }
          model->setItem(i-1, j, item);
        }
        query->next();
      }
      setEnabledPlateButtons(false);
      ui->tableViewPlates->setModel(model);
      ui->tableViewPlates->resizeRowsToContents();
      ui->tableViewPlates->resizeColumnsToContents();
    } else {
      ui->labelPlateStatus->setText("Статус: Ошибка запроса");
    }
  } else {
    ui->labelPlateStatus->setText("Статус: Ошибка соединения");
  }
  ui->tableViewPlates->setColumnWidth(2,250);
  ui->tableViewPlates->setColumnWidth(3,100);
  ui->tableViewPlates->setColumnWidth(0,80);
  ui->tableViewPlates->setColumnWidth(1,80);
}

void Administrator::on_pushButtonMove_clicked()
{

}

void Administrator::on_pushButtonPlateRemove_clicked()
{
  QSqlDatabase db = getConnection("Plates");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    QString req = "DELETE FROM `umfdb`.`Umfs` WHERE idUmf = '"+QString::number(ids[PlatesModelIndex[0]+1])+"';";
    if(query->exec(req)){
      ui->labelPlateStatus->setText("Статус: Всё хорошо");
      printTablePlates();
    } else {
      ui->labelPlateStatus->setText("Статус: Ошибка"+query->lastError().text());
    }
  }
}

void Administrator::on_pushButtonRed_clicked()
{
  QSqlDatabase db = getConnection("Plates");
  db.open();
  QSqlQuery *query = new QSqlQuery(db);
  if(db.isOpen()){
    qDebug() << red[idUmfs[PlatesModelIndex[0]+1]];
    if(red[PlatesModelIndex[0]+1] != 1){
      //red[ids[PlatesModelIndex[0]]] = 1;
      QString req = "UPDATE `umfdb`.`Umfs` SET `Color` = 'red' WHERE `idUmf` ='"+QString::number(idUmfs[PlatesModelIndex[0]+1])+"';";
      if(query->exec(req)){
        ui->labelPlateStatus->setText("Статус: Всё хорошо");
        printTablePlates();
      } else {
        qDebug() << req;
        ui->labelPlateStatus->setText("Статус: Ошибка"+query->lastError().text());
      }
    } else {
      QString req = "UPDATE `umfdb`.`Umfs` SET `Color` = 'white' WHERE `idUmf` ='"+QString::number(idUmfs[PlatesModelIndex[0]+1])+"';";
      if(query->exec(req)){
        ui->labelPlateStatus->setText("Статус: Всё хорошо");
        printTablePlates();
      } else {
        qDebug() << req;
        ui->labelPlateStatus->setText("Статус: Ошибка"+query->lastError().text());
      }
    }
  }
}
