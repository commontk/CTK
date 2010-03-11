//
// QT includes
#include <QApplication>

// ctk includes
#include "qCTKDCMTKQueryRetrieveWidget.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  qCTKDCMTKQueryRetrieveWidget queryRetrieve;
  queryRetrieve.show();
  return app.exec();
}
