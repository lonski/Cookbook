#include "przepisy.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Przepisy w;
  w.show();
  
  return a.exec();
}
