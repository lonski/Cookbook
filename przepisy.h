#ifndef PRZEPISY_H
#define PRZEPISY_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include "sqlite3.h"

#include "db/database.h"

namespace Ui {
  class Przepisy;
}

class Przepisy : public QMainWindow
{
  Q_OBJECT

  void loadMenu();
  
public:
  string dbfile;
  QString activeRec;

  void rmEcsChars(string& s);

  explicit Przepisy(QWidget *parent = 0);
  ~Przepisy();
private slots:
  void on_actionEdycja_triggered();
  void on_actionNowa_sekcja_przepis_w_triggered();
  void on_actionNowyPrzep_triggered();
  void on_tMenu_itemSelectionChanged();
  void on_actionUsu_sekcj_triggered();

  void on_actionUsu_przepis_triggered();

  void on_actionZmie_nazw_triggered();

  void on_actionZmie_nazw_przepisu_triggered();

  void on_leFind_textChanged(const QString &arg1);

  void on_toolButton_clicked();

  void on_toolButton_2_clicked();

private:
  void editing(bool e);
  Database *db;
  Ui::Przepisy *ui;
};

#endif // PRZEPISY_H
