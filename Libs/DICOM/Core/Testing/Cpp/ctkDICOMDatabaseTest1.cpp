
// Qt includes
#include <QApplication>
#include <QDir>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDICOMDatabase database;
  QDir databaseDirectory = QDir::temp();
  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  database.openDatabase(databaseFile.absoluteFilePath());

  if (!database.lastError().isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
    }

  if (!database.database().isValid())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << "invalid sql database" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.isInMemory())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << "database should not be in memory" << std::endl;
    return EXIT_FAILURE;    
    }

  if (database.databaseFilename() != databaseFile.absoluteFilePath())
    {
    std::cerr << "ctkDICOMDatabase::databaseFilename() failed: "
              << qPrintable( database.databaseFilename()) << std::endl;
    return EXIT_FAILURE;
    }

  if (QDir(database.databaseDirectory()) != databaseDirectory)
    {
    std::cerr << "ctkDICOMDatabase::databaseDirectory() failed"
              << qPrintable(database.databaseDirectory()) << std::endl;
    return EXIT_FAILURE;
    }

  bool res = database.initializeDatabase();
  
  if (!res)
    {
    std::cerr << "ctkDICOMDatabase::initializeDatabase() failed." << std::endl;
    return EXIT_FAILURE;
    }

  // check if it doesn't crash
  database.pathForDataset(0);
  database.insert(0, true, true);
  database.insert(0, true, false);
  database.insert(0, false, false);
  database.insert(0, false, true);

  database.closeDatabase();
  database.initializeDatabase();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
