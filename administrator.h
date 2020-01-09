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

  void setEnabled(bool enable);

  void on_calendarWidget_selectionChanged();

  void on_textEditControllerID_textChanged();

  void on_calendarWidget_clicked(const QDate &date);

private:
  int ModelIndex[2];
  Ui::Administrator *ui;
  void printRequest();
  void printTable();
  void addToTableControllers();
};

#endif // ADMINISTRATOR_H
