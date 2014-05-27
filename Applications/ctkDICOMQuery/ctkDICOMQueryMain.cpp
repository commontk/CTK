/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/


/* Notes:
 *
 * This program is a test driver for the DcmScu class and puts the results
 * in an sqlite database.  This command will query a public dicom server:
 *
 * ../CTK-build/bin/ctkDICOMQuery test.db FINDSCU MI2B2 mi2b2.slicer.org 11112
 * or this one:
 * ../CTK-build/bin/ctkDICOMQuery test.db FINDSCU DICOMSERVER dicomserver.co.uk 11112
 *
 * you can get a similar
 * functionality with this command line:
 *
 * findscu --verbose -aec MI2B2 -P -k "0010,0010=F*" mi2b2.slicer.org 11112 patqry.dcm | grep Patients
 *
 */

// Qt includes
#include <QCoreApplication>
#include <QTextStream>

// CTK includes
#include <ctkDICOMQuery.h>
#include <ctkDICOMDatabase.h>
#include "ctkLogger.h"

// DCMTK includes
#include "dcmtk/oflog/oflog.h"

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>

void print_usage()
{
  std::cerr << "Usage:\n";
  std::cerr << "  ctkDICOMQuery database callingAETitle calledAETitle host port\n";
  return;
}


/**
  *
*/
int main(int argc, char** argv)
{
  ctkLogger logger ( "org.commontk.core.Logger" );

  if (argc < 5)
  {
    print_usage();
    return EXIT_FAILURE;
  }

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  ctkDICOMDatabase myCTK;
  logger.debug ( "Opening database " + QString ( argv[1] ) );
  myCTK.openDatabase ( argv[1] );
  logger.debug ( "Last error: " + myCTK.lastError() );
  if ( myCTK.database().isOpen() )
    {
    logger.debug ( "Database is open" );
    }
  else
    {
    logger.debug ( "Database is not open" );
    }



  ctkDICOMQuery query;
  query.setCallingAETitle ( QString ( argv[2] ) );
  query.setCalledAETitle ( QString ( argv[3] ) );
  query.setHost ( QString ( argv[4] ) );
  int port;
  bool ok;
  port = QString ( argv[5] ).toInt ( &ok );
  if ( !ok )
    {
    std::cerr << "Could not convert " << argv[5] << " to an integer" << std::endl;
    print_usage();
    return EXIT_FAILURE;
    }
  query.setPort ( port );

  try
    {
    query.query ( myCTK );
    }
  catch (std::exception e)
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
