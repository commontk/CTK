#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include "qCTKDCMTKModel.h"

#include <iostream>

int qCTKDCMTKModelTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  qCTKDCMTKModel model(0);
  if (argc <= 2)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    return EXIT_SUCCESS;
    }
  if (!QFileInfo(argv[1]).exists() || 
      !QFileInfo(argv[2]).exists())
    {
    std::cerr << "Invalid sql file." << std::endl;
    return EXIT_FAILURE;
    }
  model.setDataBase(argv[1]);
  model.setDataBase(argv[2]);
  
  return EXIT_SUCCESS;
}
