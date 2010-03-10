#include <QTextStream>
#include <QApplication>

#include "qCTKDCMTK.h"

#include <iostream>
#include <cstdlib>

int qCTKDCMTKTest1(int argc, char** argv) {
  
  QApplication app(argc, argv);
  QTextStream out(stdout);
  qCTKDCMTK myCTK;
  if ( myCTK.openDatabase( argv[1]) )
    {
    out << "open db success\n";
    /// make sure it is empty and properly initialized
    myCTK.initializeDatabase();
    /// insert some sample data
    myCTK.initializeDatabase(argv[2]);
    myCTK.closeDatabase();
    }
  else
    { 
    out << "ERROR: " ;
    out << myCTK.GetLastError();
    }
  return EXIT_SUCCESS;
}

