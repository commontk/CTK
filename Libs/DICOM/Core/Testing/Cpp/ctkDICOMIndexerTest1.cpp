
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTreeView>
#include <QSqlQuery>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

// STD includes
#include <iostream>

int ctkDICOMIndexerTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  ctkDICOMDatabase database;
  ctkDICOMIndexer indexer;

  // Test ctkDICOMIndexer::addDirectory()
  // just check if it doesn't crash
  indexer.addDirectory(database, QString());
  // might work (if there are some DCM images in temp
  indexer.addDirectory(database, QDir::tempPath());
  // give an invalid destination name
  indexer.addDirectory(database, QDir::tempPath(), QDir::tempPath() + "/@#$%^&*{}[]");

  // make sure it doesn't crash
  indexer.refreshDatabase(database, QString());
  
  // make sure it doesn't crash
  indexer.refreshDatabase(database, QDir::tempPath());

  return EXIT_SUCCESS;
}
