//
// Qt includes
#include <QApplication>

// ctkDICOM includes
#include <ctkDICOMQueryRetrieveWidget.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  ctkDICOMQueryRetrieveWidget queryRetrieve;
  queryRetrieve.show();
  return app.exec();
}
