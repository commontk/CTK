
// QT includes
#include <QApplication>
#include <QPushButton>
#include <QTextStream>

// ctkDICOM includes
#include <ctkDICOMIndexer.h>
#include <ctkDICOM.h>

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{

  if (argc < 2)
    {
    std::cerr << "Usage: ctkDICOMIndexer <database.db> <sourceDir> [destDir]\n";
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  QTextStream out(stdout);

  ctkDICOMIndexer idx;

  ctkDICOM myCTK;
  if ( myCTK.openDatabase( argv[1]) )
    {
    out << "open db success\n";
    /// make sure it is empty and properly initialized
    myCTK.initializeDatabase();
    out << "init db done\n";
    if (argc > 3)
      {
      idx.addDirectory(myCTK.database(),argv[2],argv[3]);
      } 
      else 
      {
      idx.addDirectory(myCTK.database(),argv[2]);
      }
    out << "add db done\n";
    idx.refreshDatabase(myCTK.database(),argv[2]);
    out << "refresh db done\n";
    myCTK.closeDatabase();
    }
  else
    { 
    out << "ERROR: " ;
    out << myCTK.GetLastError();
    out << "\n" ;
    }
  return EXIT_SUCCESS;
}
