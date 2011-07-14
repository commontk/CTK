/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkModelTester.h"

// STD includes
#include <iostream>


/* Test from build directory:
 ./CTK-build/bin/CTKDICOMCoreCxxTests ctkDICOMModelTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
 If you want a test with a GUI, look at ctkDICOMTest2 in DICOM/Widgets
*/
int ctkDICOMModelTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);
  
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
          << " error: " << myCTK.lastError().toStdString();
    }
    /*
  QSqlQuery toto("SELECT PatientsName as 'Name tt' FROM Patients ORDER BY \"Name tt\" ASC", myCTK.database());
  qDebug() << "toto: " << myCTK.lastError() ;
  qDebug()<< toto.seek(0) << myCTK.lastError();
  qDebug() << toto.value(0).toString() << myCTK.lastError();

  QSqlQuery titi("SELECT StudyID as UID, StudyDescription as Name, ModalitiesInStudy as Scan, StudyDate as Date, AccessionNumber as Number, ReferringPhysician as Institution, ReferringPhysician as Referrer, PerformingPysiciansName as Performer FROM Studies WHERE PatientsUID='14'", myCTK.database());
  qDebug() << "titi: " << titi.seek(0) << myCTK.lastError();
  QSqlQuery tata("SELECT SeriesInstanceUID as UID, BodyPartExamined as Scan, SeriesDate as Date, AcquisitionNumber as Number FROM Series WHERE StudyInstanceUID='1.2.826.0.1.3680043.2.1125.1.73379483469717886505187028001198162'", myCTK.database());
  qDebug() << "tata: " << tata.seek(0) << myCTK.lastError();
  QSqlQuery tutu("SELECT SOPInstanceUID as UID, Filename as Name, SeriesInstanceUID as Date FROM Images WHERE SeriesInstanceUID='%1'", myCTK.database());
  qDebug() << "tutu: " << tutu.seek(0) << myCTK.lastError();
  */

    ctkModelTester tester;
    tester.setNestedInserts(true);
    tester.setThrowOnError(false);
    ctkDICOMModel model;
    tester.setModel(&model);

    model.setDatabase(myCTK.database());

    model.setDatabase(QSqlDatabase());

    model.setDatabase(myCTK.database());

    model.rowCount();

    qDebug() << model.rowCount() << model.columnCount();
    qDebug() << model.index(0,0);

    return EXIT_SUCCESS;
  }
  catch (std::exception e)
  {
    std::cerr << "Error when opening the data base file: " << argv[1]
        << " error: " << e.what();
    return EXIT_FAILURE;
  }
}
