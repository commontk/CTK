// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMQueryRetrieveWidget.h"

// CTK includes
//#include "ctkCheckableHeaderView.h"

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMCoreCxxTests ctkDICOMModelTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMQueryRetrieveWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  /*
  if (argc <= 2)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: qctkDICOMModelTest1 <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
    }
  */

  ctkDICOMQueryRetrieveWidget widget;
  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
