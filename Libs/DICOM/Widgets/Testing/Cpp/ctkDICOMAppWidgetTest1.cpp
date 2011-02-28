
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMAppWidget.h"

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMWidgetsCxxTests ctkDICOMAppWidgetTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMAppWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  ctkDICOMAppWidget appWidget;
  appWidget.setDatabaseDirectory(QDir::currentPath());
  appWidget.onAddToDatabase();
  appWidget.openImportDialog();
  appWidget.openExportDialog();
  appWidget.openQueryDialog();
  
  appWidget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
