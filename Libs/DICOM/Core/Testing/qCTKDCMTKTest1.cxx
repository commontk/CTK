#include <QTextStream>
#include <QApplication>

#include "qCTKDCMTK.h"

#include <iostream>
#include <cstdlib>

int qCTKDCMTKTest1(int argc, char * argv []) {
  
  QApplication app(argc, argv);
  QTextStream out(stdout);
  qCTKDCMTK myCTK;
  if ( myCTK.openDatabase( argv[1]) )
    {
    out << "open db success\n";
    /// make sure it is empty and properly initialized
    if (! myCTK.initializeDatabase() ) {
       out << "ERROR: basic DB init failed";
       return EXIT_FAILURE;
    };
    /// insert some sample data
    if (! myCTK.initializeDatabase(argv[2]) ) {
       out << "ERROR: sample DB init failed";
       return EXIT_FAILURE;
    };
    myCTK.closeDatabase(); 
    }
  else
    { 
    out << "ERROR: " ;
    out << myCTK.GetLastError();
    }
  return EXIT_SUCCESS;
}

