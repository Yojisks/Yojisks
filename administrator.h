#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <vector>

namespace Ui {
class Administrator;
}

class Administrator : public QWidget
{
  Q_OBJECT

public:
  explicit Administrator(QWidget *parent = 0);
  ~Administrator();

private slots:

  void on_pushButtonEnter_clicked();

  void on_tableViewControllers_clicked(const QModelIndex &index);

  void on_pushButtonRemove_clicked();

  void on_pushButtonRename_clicked();

  void on_textEdit_textChanged();

  void setEnabledControllerButtons(bool enable);

  void setEnabledPlateButtons(bool enable);

//  void on_calendarWidget_selectionChanged();

//  void on_textEditControllerID_textChanged();

  void on_calendarWidget_clicked(const QDate &date);

  void printTablePlates();

  void on_pushButtonAdd_clicked();

  void addToTablePlates();

  void on_tableViewPlates_clicked(const QModelIndex &index);

  void on_textEditPlateSearch_textChanged();

  void on_pushButtonMove_clicked();

  void on_pushButtonPlateRemove_clicked();

  void on_pushButtonRed_clicked();

private:
  int red[10000];
  int ControllerModelIndex[2];
  int PlatesModelIndex[2];
  Ui::Administrator *ui;
  void printRequest();
  void printTableControllers();
  void addToTableControllers();
};

#endif // ADMINISTRATOR_H
