/*=========================================================================

  Library:   CTK

  Copyright (c)  

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTextStream>

// CTK includes
#include <ctkDICOMRetrieve.h>
#include <ctkDICOM.h>
#include "ctkLogger.h"

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>

void print_usage()
{
  std::cerr << "Usage:\n";
  std::cerr << "  ctkDICOMRetrieve StudyUID OutputDirectory callingAETitle callingPort calledAETitle host calledPort moveDestinationAETitle\n";
  return;
}


/**
  *
*/
int main(int argc, char** argv)
{
  ctkLogger::configure();
  ctkLogger logger ( "org.commontk.dicom.DICOMRetieveApp" );
  logger.setDebug();

  if (argc < 9)
  {
    print_usage();
    return EXIT_FAILURE;
  }

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  QString StudyUID ( argv[1] );
  QDir OutputDirectory ( argv[2] );
  QString CallingAETitle ( argv[3] ); 
  bool ok;
  int CallingPort = QString ( argv[4] ).toInt ( &ok );
  if ( !ok )
    {
    std::cerr << "Could not convert " << argv[4] << " to an integer for the callingPort" << std::endl;
    print_usage();
    return EXIT_FAILURE;
    }

  QString CalledAETitle ( argv[5] ); 
  QString Host ( argv[6] ); 
  int CalledPort = QString ( argv[7] ).toInt ( &ok );
  if ( !ok )
    {
    std::cerr << "Could not convert " << argv[7] << " to an integer for the calledPoint" << std::endl;
    print_usage();
    return EXIT_FAILURE;
    }
  QString MoveDestinationAETitle ( argv[8] ); 

  ctkDICOMRetrieve retrieve;
  retrieve.setCallingAETitle ( CallingAETitle );
  retrieve.setCallingPort ( CallingPort );
  retrieve.setCalledAETitle ( CalledAETitle );
  retrieve.setCalledPort ( CalledPort );
  retrieve.setHost ( Host );
  retrieve.setMoveDestinationAETitle ( MoveDestinationAETitle );

  logger.info ( "StudyUID: " + StudyUID + "\n" 
                + "OutputDirectory: " + OutputDirectory.absolutePath() + "\n"
                + "CallingAETitle: " + CallingAETitle + "\n"
                + "CallingPort: " + QString::number ( CallingPort ) + "\n"
                + "CalledAEtitle: " + CalledAETitle + "\n"
                + "Host: " + Host + "\n"
                + "CalledPort: " + QString::number ( CalledPort ) + "\n" );


  logger.info ( "Starting to retrieve" );
  try
    {
    retrieve.retrieveStudy ( StudyUID, OutputDirectory );
    }
  catch (std::exception e)
    {
    logger.error ( "Retrieve failed" );
    return EXIT_FAILURE;
    }
  logger.info ( "Retrieve success" );
  return EXIT_SUCCESS;
}
