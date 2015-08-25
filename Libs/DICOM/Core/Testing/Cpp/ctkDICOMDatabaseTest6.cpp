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
#include <QDir>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest6( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  if (argc < 2)
    {
    std::cerr << "ctkDICOMDatabaseTest6: missing dicom filePath argument";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  QString dicomFilePath(argv[1]);

  ctkDICOMDatabase database;
  QDir databaseDirectory = QDir::temp();
  databaseDirectory.remove("ctkDICOMDatabase.sql");
  databaseDirectory.remove("ctkDICOMTagCache.sql");

  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  database.openDatabase(databaseFile.absoluteFilePath());

  bool res = database.initializeDatabase();

  if (!res)
    {
    std::cerr << "ctkDICOMDatabase::initializeDatabase() failed." << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Basic test:
  // - insert the file specified on the command line
  // - ask for name and descriptions and compare to known results
  //
  QString instanceUID("1.2.840.113619.2.135.3596.6358736.4843.1115808177.83");


  //
  // Test the pre load values feature of the database
  //

  QString preInsertDescription = database.descriptionForSeries(instanceUID);
  if (!preInsertDescription.isEmpty())
    {
      std::cerr
        << "ctkDICOMDatabase: db should return empty string for unknown "
        << " instance series description, instead got: "
        << preInsertDescription.toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  database.insert(dicomFilePath, false, false);

  QString filePath = database.fileForInstance(instanceUID);
  std::cerr << "Instance file " << filePath.toStdString() << std::endl;

  // check for descriptions
  QHash<QString,QString> descriptions (database.descriptionsForFile(filePath));
  std::cout << "\tPatient Name: "
            <<  descriptions["PatientsName"].toStdString()
            << "\n\tStudy Desciption: "
            <<  descriptions["StudyDescription"].toStdString()
            << "\n\tSeries Desciption: "
            <<  descriptions["SeriesDescription"].toStdString()
            << std::endl;

  // check for known series description
  QString knownSeriesDescription("3D Cor T1 FAST IR-prepped GRE");

  QString seriesUID = database.seriesForFile(filePath);
  QString seriesDescription = database.descriptionForSeries(seriesUID);

  if (seriesDescription != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: database should return series description of '"
              << knownSeriesDescription.toStdString()
              << "', instead returned '" << seriesDescription.toStdString()
              << "'\n\tinstanceUID = "
              << instanceUID.toStdString()
              << "\n\tseriesUID = "
              << seriesUID.toStdString()
              << std::endl;
    return EXIT_FAILURE;
    }

  // get the study and patient uids
  QString patientUID, studyUID;
  studyUID = database.studyForSeries(seriesUID);
  patientUID = database.patientForStudy(studyUID);

  // check for empty study description
  QString studyDescription = database.descriptionForStudy(studyUID);

  if (!studyDescription.isEmpty())
    {
    std::cerr << "ctkDICOMDatabase: database should return empty study"
              << " description for studyUID of "
              << studyUID.toStdString() << ", instead returned '"
              << studyDescription.toStdString() << "'"
              << std::endl;
    return EXIT_FAILURE;
    }

  // check for known patient name
  QString knownPatientName("Facial Expression");
  QString patientName = database.nameForPatient(patientUID);
  if (patientName != knownPatientName)
    {
    std::cerr << "ctkDICOMDatabase: database should return known patient name '"
              << knownPatientName.toStdString()
              << "' for patient UID of "
              << patientUID.toStdString() << ", instead returned '"
              << patientName.toStdString() << "'"
              << std::endl;
    return EXIT_FAILURE;
    }

  database.closeDatabase();

  std::cerr << "Database is in " << databaseDirectory.path().toStdString() << std::endl;

  return EXIT_SUCCESS;
}
