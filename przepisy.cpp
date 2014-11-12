#include "przepisy.h"
#include "ui_przepisy.h"

Przepisy::Przepisy(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Przepisy)
{

  ui->setupUi(this);

  dbfile = "../data/data.db";
  db = new Database(dbfile);

  QList<int> sizes;
  sizes.push_back(200);
  sizes.push_back(600);
  ui->splitter->setSizes(sizes);

  loadMenu();
}

Przepisy::~Przepisy()
{
  delete ui;
}

void Przepisy::rmEcsChars(string &s){

  string chars = "\"\'";

  for (size_t i = 0; i<s.size(); i++ ){
    size_t f = chars.find(s[i]);
    if (f < chars.size()){
      s.insert(i,chars.substr(f,1));
      i++;
    }
  }

}

void Przepisy::editing(bool e){
  ui->actionEdycja->setChecked(e);
  on_actionEdycja_triggered();
}

void Przepisy::loadMenu()
{
  ui->tMenu->clearSelection();

  SQLTable sections, children;
  sections = db->query("SELECT * FROM sections");

  SQLTable::iterator sit;
  SQLTable::iterator cit;

  ui->tMenu->clear();

  for(sit = sections.begin(); sit != sections.end(); sit++){
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0,QString::fromStdString((*sit)["name"]));
    item->setIcon(0,QIcon(":/ico/data/book.png"));

      children = db->query("SELECT * FROM children WHERE parent = '"+ (*sit)["name"] +"';");
      for (cit = children.begin(); cit != children.end(); cit++){
        QTreeWidgetItem *citem = new QTreeWidgetItem();
        citem->setText(0,QString::fromStdString((*cit)["name"]));
        citem->setIcon(0,QIcon(":/ico/data/bookmark.png"));
        item->addChild(citem);
      }

    ui->tMenu->insertTopLevelItem(0,item);
  }

}

void Przepisy::on_actionEdycja_triggered()
{
  if (!ui->actionEdycja->isChecked()){
    ui->txtBrowser->setReadOnly(true);

    string recka;
    recka = ui->txtBrowser->document()->toPlainText().toStdString();

    rmEcsChars(recka);

    SQLTable rec;
    rec = db->query("SELECT recipe FROM recipes WHERE name='"+activeRec.toStdString()+"';");
    if (rec.empty()){ //insert
        db->query("INSERT INTO recipes(name, recipe) VALUES('"+activeRec.toStdString()+"', '"+recka+"');");
    }else{ //update
        db->query("UPDATE recipes SET recipe='"+recka+"' WHERE name='"+activeRec.toStdString()+"';");
    }

  }else{
    ui->txtBrowser->setReadOnly(false);

  }
}

void Przepisy::on_actionNowa_sekcja_przepis_w_triggered()
{
  bool ok;
  QString text = QInputDialog::getText(this, "Nowa sekcja","Podaj nazwe nowej sekcji przepisów:", QLineEdit::Normal,"",&ok);
  if (ok && !text.isEmpty()){
    if (db->exists("SELECT * FROM sections WHERE name='"+text.toStdString()+"';")){
      QMessageBox::critical(this,"Błąd","Sekcja o tej nazwie juz istnieje!");
    }else{
      db->query("INSERT INTO sections(name) VALUES('"+text.toStdString()+"');");
      loadMenu();

      QTreeWidgetItem *itm;
      QList<QTreeWidgetItem*> f_itm = ui->tMenu->findItems(text,Qt::MatchRecursive);
      if (f_itm.size() > 0){
        itm = f_itm.at(0);
        ui->tMenu->setItemSelected(itm, true);
      }
    }
  }
}

void Przepisy::on_actionNowyPrzep_triggered()
{
  if (activeRec == ""){
    QMessageBox::critical(this,"Błąd","Zaznacz najpierw sekcję do której chcesz dodać przepis!");
  }else{
    bool ok;
    QString text = QInputDialog::getText(this, "Nowy przepis","Podaj nazwe nowego przepisu:", QLineEdit::Normal,"",&ok);
    if (ok && !text.isEmpty()){
      if (db->exists("SELECT * FROM children WHERE name='"+text.toStdString()+"';")){
        QMessageBox::critical(this,"Błąd","Przepis o tej nazwie juz istnieje!");
      }else{
        db->query("INSERT INTO children(name, parent) VALUES('"+text.toStdString()+"', '"+activeRec.toStdString()+"');");
        loadMenu();

        QTreeWidgetItem *itm;
        QList<QTreeWidgetItem*> f_itm = ui->tMenu->findItems(text,Qt::MatchRecursive);
        if (f_itm.size() > 0){
          itm = f_itm.at(0);
          ui->tMenu->expandItem(itm->parent());
          ui->tMenu->setItemSelected(itm, true);
          editing(true);

        }
      }
    }
  }

}


void Przepisy::on_tMenu_itemSelectionChanged()
{
  int column = 0;
  QTreeWidgetItem *item;
  if (ui->tMenu->selectedItems().size()>0){

    item = ui->tMenu->selectedItems().at(0);

    if (activeRec != item->text(column)){
      if (ui->actionEdycja->isChecked()){
        editing(false);
      }

      activeRec = item->text(column);

      SQLTable recipes;
      recipes = db->query("SELECT recipe FROM recipes WHERE name = '"+item->text(column).toStdString()+"';");

      if (!recipes.empty()){
        QString recka = QString::fromStdString(recipes[0]["recipe"]);
        ui->txtBrowser->setText(recka);
      }else{
        ui->txtBrowser->setText("");
      }
    }

  }
}

void Przepisy::on_actionUsu_sekcj_triggered()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Usunięcie sekcji","Napewno usunąć sekcję "+activeRec+"?\nZostaną usunięte wszystkie przepisy które pod nią podlegają!", QMessageBox::Yes | QMessageBox::No);

  if (reply==QMessageBox::Yes){
    db->query("DELETE FROM recipes WHERE name IN (SELECT name FROM children WHERE parent = '" +activeRec.toStdString()+ "')");
    db->query("DELETE FROM recipes WHERE name = '" +activeRec.toStdString()+ "')");
    db->query("DELETE FROM children WHERE parent='"+activeRec.toStdString()+"';");
    db->query("DELETE FROM sections WHERE name='"+activeRec.toStdString()+"';");
    loadMenu();
  }
}

void Przepisy::on_actionUsu_przepis_triggered()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Usunięcie przepisu","Napewno usunąć przepis "+activeRec+"?", QMessageBox::Yes | QMessageBox::No);

  if (reply==QMessageBox::Yes){
    db->query("DELETE FROM recipes WHERE name = '" +activeRec.toStdString()+ "'");
    db->query("DELETE FROM children WHERE name='"+activeRec.toStdString()+"';");
    loadMenu();
  }
}

void Przepisy::on_actionZmie_nazw_triggered()
{
  bool ok;
  QString newname = QInputDialog::getText(this, "Zmiana nazwy sekcji","Podaj nową nazwę sekcji:", QLineEdit::Normal,activeRec,&ok);
  if (ok && !newname.isEmpty()){
    db->query("UPDATE children SET parent='"+ newname.toStdString() +"' WHERE parent='"+activeRec.toStdString()+"';");
    db->query("UPDATE recipes SET name='"+newname.toStdString()+"' WHERE name = '"+activeRec.toStdString()+"';");
    db->query("UPDATE sections SET name='"+newname.toStdString()+"' WHERE name = '"+activeRec.toStdString()+"';");

    loadMenu();
  }

}

void Przepisy::on_actionZmie_nazw_przepisu_triggered()
{
  bool ok;
  QString newname = QInputDialog::getText(this, "Zmiana nazwy przepisu","Podaj nową nazwę przepisu:", QLineEdit::Normal,activeRec,&ok);
  if (ok && !newname.isEmpty()){
    db->query("UPDATE recipes SET name='"+newname.toStdString()+"' WHERE name = '"+activeRec.toStdString()+"';");
    db->query("UPDATE children SET name='"+newname.toStdString()+"' WHERE name = '"+activeRec.toStdString()+"';");

    loadMenu();
  }
}

void Przepisy::on_leFind_textChanged(const QString &arg1)
{
  ui->tMenu->clearSelection();
  QTreeWidgetItem *itm;
  QList<QTreeWidgetItem*> f_itm = ui->tMenu->findItems(arg1,Qt::MatchStartsWith | Qt::MatchRecursive);
  qDebug() << f_itm.size();
  if (f_itm.size() > 0){
    itm = f_itm.at(0);
    ui->tMenu->expandItem(itm->parent());
    ui->tMenu->setItemSelected(itm, true);
  }

}

void Przepisy::on_toolButton_clicked()
{
  ui->tMenu->collapseAll();
}

void Przepisy::on_toolButton_2_clicked()
{
  ui->tMenu->expandAll();
}
