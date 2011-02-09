
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QTimer>
#include <QTreeView>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkModelTester.h"

// CTK includes
#include "ctkCheckableHeaderView.h"

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMCoreCxxTests ctkDICOMModelTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMModelTest2( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  if (argc <= 2)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: qctkDICOMModelTest1 <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
    }
  
  try
  {
    ctkDICOMDatabase myCTK( argv[1] );

    if (!myCTK.initializeDatabase(argv[2]))
    {
      std::cerr << "Error when initializing the data base: " << argv[2]
          << " error: " << myCTK.GetLastError().toStdString();
    }
 
    ctkDICOMModel model;
    model.setDatabase(myCTK.database());

    QWidget topLevel;
    QTreeView viewer;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(&viewer);
    topLevel.setLayout(layout);
    viewer.setModel(&model);
    
    QHeaderView* previousHeaderView = viewer.header();
    qDebug() << "previous: " << previousHeaderView->isHidden();
    ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, &viewer);
    headerView->setClickable(previousHeaderView->isClickable());
    headerView->setMovable(previousHeaderView->isMovable());
    headerView->setHighlightSections(previousHeaderView->highlightSections());
    headerView->setPropagateToItems(true);
    viewer.setHeader(headerView);
    qDebug() << "new: " << headerView->isHidden();
    topLevel.show();
    if (argc <= 3 || QString(argv[3]) != "-I")
      {
      QTimer::singleShot(200, &app, SLOT(quit()));
      }
    return app.exec();
  }
  catch (std::exception e)
    {
    std::cerr << "Error when opening the data base file: " << argv[1]
        << " error: " << e.what();
    return EXIT_FAILURE;
    }
}
