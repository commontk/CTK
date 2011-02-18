// Qt includes
#include <QCoreApplication>
#include <QStringList>

// ctkDICOMCore includes
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>
#include <cstdlib>

void ctkDICOMTesterTest2PrintUsage()
{
  std::cout << " ctkDICOMTesterTest2 images" << std::endl;
}

int ctkDICOMTesterTest2(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  arguments.pop_front();
  if (!arguments.count())
    {
    ctkDICOMTesterTest2PrintUsage();
    return EXIT_FAILURE;
    }

  ctkDICOMTester tester;
  tester.startDCMQRSCP();

  bool res = tester.storeData(arguments);

  if (!res)
    {
    std::cout << "Can't store data" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

