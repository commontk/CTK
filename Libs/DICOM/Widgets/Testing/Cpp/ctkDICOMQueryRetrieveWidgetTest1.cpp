// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMQueryRetrieveWidget.h"

// STD includes
#include <iostream>

int ctkDICOMQueryRetrieveWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
 
  ctkDICOMQueryRetrieveWidget widget;
  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
