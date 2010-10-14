
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QTreeView>
#include <QSqlQuery>

// ctkDICOMCore includes
#include "ctkDICOM.h"
#include "ctkDICOMModel.h"
#include "ctkModelTester.h"

// STD includes
#include <iostream>


/* Test from build directory:
 ./CTK-build/bin/CTKDICOMCoreCxxTests ctkDICOMModelTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMModelTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  if (argc <= 2)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: qctkDICOMModelTest1 <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
    }
  
  ctkDICOM myCTK;
  try
  {
    myCTK.openDatabase( argv[1] );
  }
  catch (std::exception e)
  {
    std::cerr << "Error when opening the data base file: " << argv[1] 
              << " error: " << e.what();
    return EXIT_FAILURE;
  }
  if (!myCTK.initializeDatabase(argv[2]))
    {
    std::cerr << "Error when initializing the data base: " << argv[2] 
              << " error: " << myCTK.GetLastError().toStdString();
    }
    /*
  QSqlQuery toto("SELECT PatientsName as 'Name tt' FROM Patients ORDER BY \"Name tt\" ASC", myCTK.database());
  qDebug() << "toto: " << myCTK.GetLastError() ;
  qDebug()<< toto.seek(0) << myCTK.GetLastError();
  qDebug() << toto.value(0).toString() << myCTK.GetLastError();

  QSqlQuery titi("SELECT StudyID as UID, StudyDescription as Name, ModalitiesInStudy as Scan, StudyDate as Date, AccessionNumber as Number, ReferringPhysician as Institution, ReferringPhysician as Referrer, PerformingPysiciansName as Performer FROM Studies WHERE PatientsUID='14'", myCTK.database());
  qDebug() << "titi: " << titi.seek(0) << myCTK.GetLastError();
  QSqlQuery tata("SELECT SeriesInstanceUID as UID, BodyPartExamined as Scan, SeriesDate as Date, AcquisitionNumber as Number FROM Series WHERE StudyInstanceUID='1.2.826.0.1.3680043.2.1125.1.73379483469717886505187028001198162'", myCTK.database());
  qDebug() << "tata: " << tata.seek(0) << myCTK.GetLastError();
  QSqlQuery tutu("SELECT Filename as UID, Filename as Name, SeriesInstanceUID as Date FROM Images WHERE SeriesInstanceUID='%1'", myCTK.database());
  qDebug() << "tutu: " << tutu.seek(0) << myCTK.GetLastError();
  */

  ctkModelTester tester;
  tester.setNestedInserts(true);
  tester.setThrowOnError(false);
  ctkDICOMModel model;
  tester.setModel(&model);

  model.setDatabase(myCTK.database());
  
  model.setDatabase(QSqlDatabase());
 
  model.setDatabase(myCTK.database());

  QTreeView viewer;
  viewer.setModel(&model);
  viewer.setSortingEnabled(true);

  model.rowCount();
  qDebug() << model.rowCount() << model.columnCount();
  qDebug() << model.index(0,0);
  viewer.show();
  if (argc > 3 && QString(argv[3]) == "-I")
    {
    return app.exec();
    }
  return EXIT_SUCCESS;
}
