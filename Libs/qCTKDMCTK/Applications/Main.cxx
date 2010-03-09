
// QT includes
#include <QApplication>
#include <QPushButton>
// STD includes
//#include <cstdlib>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  QPushButton b("Hello world");
  b.show();
  return app.exec();
}
