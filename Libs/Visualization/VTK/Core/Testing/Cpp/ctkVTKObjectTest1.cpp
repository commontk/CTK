
// Qt includes
#include <QCoreApplication>

// CTKVTK includes
#include "ctkVTKObjectTestHelper.h"

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKObjectTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkVTKObjectTest objectTest;
  if (!objectTest.test())
    {
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

