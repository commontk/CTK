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

// Qt includes
#include <QTextStream>
#include <QCoreApplication>
#include <QProcess>

// STD includes
#include <iostream>
#include <cstdlib>

//
//  This test performs a full dicom store, query, and retrieve.
//
//  To recreate the parts, from the CTK-superbuild directory, run:
//
//
//  ./CMakeExternals/Install/bin/dcmqrscp -c ./CTK-build/Testing/Temporary/dcmqrscp.cfg -d -v
//
//  ./CMakeExternals/Install/bin/storescu -aec CTK_AE -aet CTK_AE localhost 11112 ./CMakeExternals/Source/CTKData/Data/DICOM/MRHEAD/*.IMA
//
//  ./CMakeExternals/Install/bin/findscu -aet CTK_AE -aec COMMONTK -P -k 0010,0010=\* localhost 11112 patqry.dcm
//
//  ./CTK-build/bin/ctkDICOMQuery /tmp/test.db CTK_AE CTK_AE localhost 11112
//
//
//  ./CTK-build/bin/ctkDICOMRetrieve 1.2.840.113619.2.135.3596.6358736.5118.1115807980.182  /tmp/hoot CTK_AE 11113 CTK_AE localhost 11112 CTK_CLIENT_AE
//
//  As invoked by ctest:
//
//  % ./CTK-build/bin/CTKApplicationCppTests ctkDICOMApplicationTest1 \
//      ./CMakeExternals/Install/bin/dcmqrscp ./CTK-build/Testing/Temporary/dcmqrscp.cfg \
//          ./CMakeExternals/Source/CTKData/Data/DICOM/MRHEAD/000055.IMA \
//          ./CMakeExternals/Source/CTKData/Data/DICOM/MRHEAD/000056.IMA \
//      ./CMakeExternals/Install/bin/storescu \
//      ./CTK-build/bin/ctkDICOMQuery \
//      ./CTK-build/Testing/Temporary/ctkDICOMApplicationTest1.db \
//      ./CTK-build/bin/ctkDICOMRetrieve \
//      ./CTK-build/Testing/Temporary/ctkDICOMRetrieveStorage
//

int main(int argc, char * argv []) {

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  if ( argc < 10 )
  {
    out << "ERROR: invalid arguments.  Should be:\n";
    out << " ctkDICOMApplicationTest1 <dcmqrscp> <configfile> <dicomData1> <dcmData2> <storescu> <ctkDICOMQuery> <ctkDICOMRetrieve> <retrieveDirectory>\n";
    return EXIT_FAILURE;
  }

  QString dcmqrscp_exe  (argv[1]);
  QString dcmqrscp_cfg  (argv[2]);
  QString dicomData1  (argv[3]);
  QString dicomData2  (argv[4]);
  QString storescu_exe  (argv[5]);
  QString ctkDICOMQuery_exe  (argv[6]);
  QString ctkDICOMQuery_db_file  (argv[7]);
  QString ctkDICOMRetrieve_exe  (argv[8]);
  QString ctkDICOMRetrieve_directory  (argv[9]);

  //
  // first, start the server process
  //

  QProcess *dcmqrscp = new QProcess(0);
  QStringList dcmqrscp_args;
  dcmqrscp_args << "--config" << dcmqrscp_cfg;
  dcmqrscp_args << "--debug" << "--verbose";
  dcmqrscp_args << "11112";

  try
  {
    out << "starting server" << dcmqrscp_exe << "\n";
    out << "with args " << dcmqrscp_args.join(" ") << "\n";
    dcmqrscp->start(dcmqrscp_exe, dcmqrscp_args);
    dcmqrscp->waitForStarted();
  }
  catch (std::exception e)
  {
    out << "ERROR: could not start server" << e.what();
    return EXIT_FAILURE;
  }


  //
  // now push some dicom data in using storescu
  //

  QProcess *storescu = new QProcess(0);
  QStringList storescu_args;
  storescu_args << "-aec" << "CTK_AE";
  storescu_args << "-aet" << "CTK_AE";
  storescu_args << "localhost" << "11112";
  storescu_args << dicomData1;
  storescu_args << dicomData2;

  try
  {
    out << "running client" << storescu_exe << "\n";
    out << "with args" << storescu_args.join(" ") << "\n";
    storescu->start(storescu_exe, storescu_args);
    storescu->waitForFinished();
    out << "storescu Finished.\n";
    out << "Standard Output:\n";
    out << storescu->readAllStandardOutput();
    out << "Standard Error:\n";
    out << storescu->readAllStandardError();
  }
  catch (std::exception e)
  {
    out << "ERROR: could not start client" << e.what();
    return EXIT_FAILURE;
  }

  //
  // now query the server to see if the data arrived okay
  // - our database file will be updated with metadata from the query
  //

  QProcess *ctkDICOMQuery = new QProcess(0);
  QStringList ctkDICOMQuery_args;
  ctkDICOMQuery_args << ctkDICOMQuery_db_file;
  ctkDICOMQuery_args << "CTK_AE" << "CTK_AE";
  ctkDICOMQuery_args << "localhost" << "11112";

  try
  {
    out << "running client" << ctkDICOMQuery_exe << "\n";
    out << "with args" << ctkDICOMQuery_args.join(" ") << "\n";
    ctkDICOMQuery->start(ctkDICOMQuery_exe, ctkDICOMQuery_args);
    ctkDICOMQuery->waitForFinished();
    out << "ctkDICOMQuery Finished.\n";
    out << "Standard Output:\n";
    out << ctkDICOMQuery->readAllStandardOutput();
    out << "Standard Error:\n";
    out << ctkDICOMQuery->readAllStandardError();
  }
  catch (std::exception e)
  {
    out << "ERROR: could not start client" << e.what();
    return EXIT_FAILURE;
  }


  //
  // now do a retrieve into our download directory
  //

  // this is the study id of the dicom files we load from CTKData
  QString studyUID("1.2.840.113619.2.135.3596.6358736.5118.1115807980.182");

  QProcess *ctkDICOMRetrieve = new QProcess(0);
  QStringList ctkDICOMRetrieve_args;
  ctkDICOMRetrieve_args << studyUID;
  ctkDICOMRetrieve_args << ctkDICOMRetrieve_directory;
  ctkDICOMRetrieve_args << "CTK_AE" << "11113";
  ctkDICOMRetrieve_args << "CTK_AE";
  ctkDICOMRetrieve_args << "localhost" << "11112" << "CTK_CLIENT_AE";

  try
  {
    out << "running client" << ctkDICOMRetrieve_exe << "\n";
    out << "with args" << ctkDICOMRetrieve_args.join(" ") << "\n";
    ctkDICOMRetrieve->start(ctkDICOMRetrieve_exe, ctkDICOMRetrieve_args);
    ctkDICOMRetrieve->waitForFinished();
    out << "ctkDICOMRetrieve Finished.\n";
    out << "Standard Output:\n";
    out << ctkDICOMRetrieve->readAllStandardOutput();
    out << "Standard Error:\n";
    out << ctkDICOMRetrieve->readAllStandardError();
  }
  catch (std::exception e)
  {
    out << "ERROR: could not start client" << e.what();
    return EXIT_FAILURE;
  }


  //
  // clients are finished, not kill server and print output
  //
  try
  {
    dcmqrscp->kill();
    dcmqrscp->waitForFinished();
    out << "dcmqrscp Finished.\n";
    out << "Standard Output:\n";
    out << dcmqrscp->readAllStandardOutput();
    out << "Standard Error:\n";
    out << dcmqrscp->readAllStandardError();
  }
  catch (std::exception e)
  {
    out << "ERROR: could not start client" << e.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

