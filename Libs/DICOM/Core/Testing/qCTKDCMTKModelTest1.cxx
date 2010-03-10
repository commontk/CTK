#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QTreeView>

#include "qCTKDCMTKModel.h"

#include <iostream>

int qCTKDCMTKModelTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  qCTKDCMTKModel model(0);
  if (argc <= 1)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    return EXIT_SUCCESS;
    }
  if (!QFileInfo(argv[1]).exists())
    {
    std::cerr << "Invalid sql file." << std::endl;
    return EXIT_FAILURE;
    }
    
  QTreeView viewer(0);
  viewer.setModel(&model);
  
  model.setDataBase(argv[1]);

  model.rowCount();
  qDebug() << model.rowCount() << model.columnCount();
  qDebug() << model.index(0,0);
  viewer.show();
  return app.exec();
}
