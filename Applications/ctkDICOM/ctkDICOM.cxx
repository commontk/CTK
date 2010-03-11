//
// QT includes
#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>

// ctk includes
#include "qCTKDCMTKQueryRetrieveWidget.h"
#include "qCTKDCMTKQueryWidget.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  qCTKDCMTKQueryRetrieveWidget queryRetrieve;
  qCTKDCMTKQueryWidget query;
  //query.setParent(queryRetrieve.findChild<QFrame *>("queryFrame"));
  queryRetrieve.show();
  return app.exec();
}
