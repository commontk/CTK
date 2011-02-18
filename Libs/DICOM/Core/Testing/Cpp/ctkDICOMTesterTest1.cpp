// Qt includes
#include <QCoreApplication>

// ctkDICOMCore includes
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>
#include <cstdlib>

void printUsage()
{
  std::cout << " ctkDICOMTesterTest1 [<dcmqrscp>] [<configfile>]" << std::endl;
}

int ctkDICOMTesterTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

  ctkDICOMTester tester;
  
  if (argc > 1)
    {
    tester.setDCMQRSCPExecutable(argv[1]);
    }
  if (argc > 2)
    {
    tester.setDCMQRSCPConfigFile(argv[2]);
    }

  QProcess* process = tester.startDCMQRSCP();
  if (!process)
    {
    std::cerr << "Failed to start dcmqrscp" << argv[1] << " with config file:"
              << argv[2] << std::endl;
    return EXIT_FAILURE;
    }
  tester.stopDCMQRSCP();

  QProcess* process2 = tester.startDCMQRSCP();
  if (!process2 || process2 == process)
    {
    std::cerr << "Failed to start dcmqrscp" << argv[1] << " with config file:"
              << argv[2] << std::endl;
    return EXIT_FAILURE;
    }
  QProcess* process3 = tester.startDCMQRSCP();
  if (!process3 || process3 != process2)
    {
    std::cerr << "Failed to start dcmqrscp" << argv[1] << " with config file:"
              << argv[2] << std::endl;
    return EXIT_FAILURE;
    }
  tester.stopDCMQRSCP();
  tester.stopDCMQRSCP();

  return EXIT_SUCCESS;
}

