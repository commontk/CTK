//
// QT includes
#include <QApplication>

// ctk includes
#include "qCTKDCMTKQueryWidget.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  qCTKDCMTKQueryWidget query;
  query.show();
  return app.exec();
}
