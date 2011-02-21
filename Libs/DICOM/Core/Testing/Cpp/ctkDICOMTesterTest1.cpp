// Qt includes
#include <QCoreApplication>
#include <QFileInfo>

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
    if (tester.dcmqrscpExecutable() != argv[1])
      {
       std::cerr << __LINE__
                 << ": Failed to set dcmqrscp: " << argv[1]
                 << " value:" << qPrintable(tester.dcmqrscpExecutable())
                 << std::endl;
       return EXIT_FAILURE;
      }
    }
  if (argc > 2)
    {
    tester.setDCMQRSCPConfigFile(argv[2]);
    if (tester.dcmqrscpConfigFile() != argv[2])
      {
       std::cerr << __LINE__
                 << ": Failed to set dcmqrscp config file: " << argv[2]
                 << " value:" << qPrintable(tester.dcmqrscpConfigFile())
                 << std::endl;
       return EXIT_FAILURE;
      }
    }

  QString dcmqrscp(tester.dcmqrscpExecutable());
  QString dcmqrscpConf(tester.dcmqrscpConfigFile());

  if (!QFileInfo(dcmqrscp).exists() ||
      !QFileInfo(dcmqrscp).isExecutable())
    {
    std::cerr << __LINE__
              << ": Wrong dcmqrscp executable: " << qPrintable(dcmqrscp)
              << std::endl;
    }

  if (!QFileInfo(dcmqrscpConf).exists() ||
      !QFileInfo(dcmqrscpConf).isReadable())
    {
    std::cerr << __LINE__
              << ": Wrong dcmqrscp executable: " << qPrintable(dcmqrscp)
              << std::endl;
    }

  QProcess* process = tester.startDCMQRSCP();
  if (!process)
    {
    std::cerr << __LINE__
              << ": Failed to start dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:" << qPrintable(dcmqrscpConf) << std::endl;
    return EXIT_FAILURE;
    }
  bool res = tester.stopDCMQRSCP();
  if (!res)
    {
    std::cerr << __LINE__
              << ": Failed to stop dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:" << qPrintable(dcmqrscpConf) << std::endl;
    return EXIT_FAILURE;
    }

  process = tester.startDCMQRSCP();
  if (!process)
    {
    std::cerr << __LINE__
              << ": Failed to start dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:" << qPrintable(dcmqrscpConf)
              << std::endl;
    return EXIT_FAILURE;
    }
  process = tester.startDCMQRSCP();
  if (process)
    {
    std::cerr << __LINE__
              << ": Failed to start dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:"<< qPrintable(dcmqrscpConf) << std::endl;
    return EXIT_FAILURE;
    }
  res = tester.stopDCMQRSCP();
  if (!res)
    {
    std::cerr << __LINE__
              << ": Failed to stop dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:" << qPrintable(dcmqrscpConf) << std::endl;
    return EXIT_FAILURE;
    }
  // there should be no process to stop
  res = tester.stopDCMQRSCP();
  if (res)
    {
    std::cerr << __LINE__
              << ": Failed to stop dcmqrscp: " << qPrintable(dcmqrscp)
              << " with config file:" << qPrintable(dcmqrscpConf) << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

