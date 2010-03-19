
// QT includes
#include <QApplication>
#include <QPushButton>
#include <QTextStream>

// CTKDICOM includes
#include <qCTKDCMTKIndexer.h>
#include <qCTKDCMTK.h>

// STD includes
#include <cstdlib>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QTextStream out(stdout);

  qCTKDCMTKIndexer idx;

  qCTKDCMTK myCTK;
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
    }
  return EXIT_SUCCESS;
}
