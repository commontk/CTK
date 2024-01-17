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
#include <QFileInfo>
#include <QProcess>

// CTK includes
#include "ctkCoreTestingMacros.h"

// STD includes
#include <iostream>
#include <cstdlib>

//
//  This test performs a full dicom store, query, and retrieve.
//
//  To recreate the parts, from the CTK-superbuild directory, run:
//
//
//  ./DCMTK-build/bin/dcmqrscp -c ./CTK-build/Testing/Temporary/dcmqrscp.cfg -d -v
//
//  ./DCMTK-build/bin/bin/storescu -aec CTK_AE -aet CTK_AE localhost 11112 ./CTKData/Data/DICOM/MRHEAD/*.IMA
//
//  ./DCMTK-build/bin/bin/findscu -aet CTK_AE -aec COMMONTK -P -k 0010,0010=\* localhost 11112 patqry.dcm
//
//  ./CTK-build/bin/ctkDICOMQuery /tmp/test.db CTK_AE CTK_AE localhost 11112
//
//
//  ./CTK-build/bin/ctkDICOMRetrieve 1.2.840.113619.2.135.3596.6358736.5118.1115807980.182  /tmp/hoot CTK_AE 11113 CTK_AE localhost 11112 CTK_CLIENT_AE
/*
//
//  As invoked by ctest:
//
//  % ./CTK-build/bin/CTKApplicationCppTests ctkDICOMApplicationTest1 \
//      ./DCMTK-build/bin/bin/dcmqrscp ./CTK-build/Testing/Temporary/dcmqrscp.cfg \
//          ./CTKData/Data/DICOM/MRHEAD/000055.IMA \
//          ./CTKData/Data/DICOM/MRHEAD/000056.IMA \
//      ./DCMTK-build/bin/bin/storescu \
//      ./CTK-build/bin/ctkDICOMQuery \
//      ./CTK-build/Testing/Temporary/ctkDICOMApplicationTest1.db \
//      ./CTK-build/bin/ctkDICOMRetrieve \
//      ./CTK-build/Testing/Temporary/ctkDICOMRetrieveStorage
//
*/

namespace
{

//-----------------------------------------------------------------------------
enum WaitType
{
  WaitForStarted, //!< Blocks until the process has started
  WaitForFinished, //!< Blocks until the process has finished
};

//-----------------------------------------------------------------------------
int wait_for_finished(QTextStream& out, const QString& type, QProcess* process, bool kill=false)
{
  try
    {
    QString programName = QFileInfo(process->program()).baseName();

    if (kill)
      {
      process->kill();
      }

    process->waitForFinished();

    out << "------------------------------------------------------------------------------\n";
    out << "------------------------------------------------------------------------------\n";
    out << "------------------------------------------------------------------------------\n";
    out << type << " " << programName << " Finished.\n";

    out << "Process Killed: " << (kill ? "yes" : "no") << "\n";

    out << "Process ExitCode: ";
    if (process->exitCode() == QProcess::NormalExit)
      {
      out << "NormalExit (" << static_cast<int>(QProcess::NormalExit) << ")\n";
      }
    else
      {
      out << "CrashExit (" << static_cast<int>(QProcess::CrashExit) << ")\n";
      }

    if (process->exitCode() == QProcess::CrashExit)
      {
      out << "Process State: ";
      switch(static_cast<int>(process->error()))
        {

        #define _QProcessErrorToString(ERROR) \
          case QProcess::ERROR: \
            out << #ERROR << " (" << static_cast<int>(QProcess::ERROR) << ")\n"; \
            break;

        _QProcessErrorToString(FailedToStart);
        _QProcessErrorToString(Crashed);
        _QProcessErrorToString(Timedout);
        _QProcessErrorToString(WriteError);
        _QProcessErrorToString(ReadError);
        _QProcessErrorToString(UnknownError);

        #undef _QProcessErrorToString
        }
      }
    out << "Standard Output:\n";
    out << process->readAllStandardOutput();
    out << "Standard Error:\n";
    out << process->readAllStandardError();

    if (kill)
      {
      return EXIT_SUCCESS;
      }
    else
      {
      return process->exitStatus() == QProcess::NormalExit ? process->exitCode() : EXIT_FAILURE;
      }
    }
  catch (const std::exception& e)
    {
    out << "ERROR: exception ocurred in wait_for_finished(): " << e.what();
    return EXIT_FAILURE;
    }
}

//-----------------------------------------------------------------------------
int run_process(
    QTextStream& out, const QString& type, WaitType wait_type,
    QProcess* process, const QString& exectuable, const QStringList& arguments)
{
  try
  {
    QString programName = QFileInfo(exectuable).baseName();

    out << "------------------------------------------------------------------------------\n";
    if (type == "server")
      {
      out << "  ____  _____ ______     _______ ____  " << "\n";
      out << " / ___|| ____|  _ \\ \\   / / ____|  _ \\ " << "\n";
      out << " \\___ \\|  _| | |_) \\ \\ / /|  _| | |_) |" << "\n";
      out << "  ___) | |___|  _ < \\ V / | |___|  _ < " << "\n";
      out << " |____/|_____|_| \\_\\ \\_/  |_____|_| \\_\\" << "\n";
      }

    else if (type == "client")
      {
      out << "  ____ _     ___ _____ _   _ _____ " << "\n";
      out << " / ___| |   |_ _| ____| \\ | |_   _|" << "\n";
      out << "| |   | |    | ||  _| |  \\| | | |  " << "\n";
      out << "| |___| |___ | || |___| |\\  | | |  " << "\n";
      out << " \\____|_____|___|_____|_| \\_| |_|  " << "\n";
      }
    out << "------------------------------------------------------------------------------\n";
    out << "starting " << type << " " << exectuable << "\n";
    out << "with args " << arguments.join(" ") << "\n";
    process->start(exectuable, arguments);
    switch(wait_type)
      {
      case WaitForStarted:
        process->waitForStarted();
        out << programName << " Started.\n";
        return process->state() == QProcess::Running ? EXIT_SUCCESS : EXIT_FAILURE;

      case WaitForFinished:
        return wait_for_finished(out, type, process);

      default:
        return EXIT_FAILURE;
      }
  }
  catch (const std::exception& e)
  {
    out << "ERROR: exception ocurred in run_process(): " << e.what();
    return EXIT_FAILURE;
  }
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int main(int argc, char * argv []) {

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (arguments.count() != 9)
  {
    std::cerr << "ERROR: invalid or missing arguments.\n\n"
              << "Usage: " << qPrintable(testName)
              << " <dcmqrscp> <configfile>"
                 " <dicomData1>"
                 " <dicomData2>"
                 " <storescu>"
                 " <ctkDICOMQuery> <databaseFile>"
                 " <ctkDICOMRetrieve> <retrieveDirectory>\n";
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
  {
    CHECK_EXIT_SUCCESS(
          run_process(out, "server", WaitForStarted, dcmqrscp, dcmqrscp_exe,
                      {
                        "--config", dcmqrscp_cfg,
                        "--debug",
                        "--verbose",
                        "11112"
                      }));
  }


  //
  // now push some dicom data in using storescu
  //

  QProcess *storescu = new QProcess(0);
  {
    CHECK_EXIT_SUCCESS(
          run_process(out, "client", WaitForFinished, storescu, storescu_exe,
                      {
                        "-aec", "CTK_AE",
                        "-aet", "CTK_AE",
                        "localhost", "11112",
                        dicomData1,
                        dicomData2,
                      }));
  }


  //
  // now query the server to see if the data arrived okay
  // - our database file will be updated with metadata from the query
  //

  QProcess *ctkDICOMQuery = new QProcess(0);
  {
    // Usage:
    //   ctkDICOMQuery database callingAETitle calledAETitle host port

    CHECK_EXIT_SUCCESS(
          run_process(out, "client", WaitForFinished, ctkDICOMQuery, ctkDICOMQuery_exe,
                      {
                        ctkDICOMQuery_db_file,
                        "CTK_AE",
                        "CTK_AE",
                        "localhost",
                        "11112"
                      }));
  }


  //
  // now do a retrieve into our download directory
  //

  // this is the study id of the dicom files we load from CTKData
  QString studyUID("1.2.840.113619.2.135.3596.6358736.5118.1115807980.182");

  QProcess *ctkDICOMRetrieve = new QProcess(0);
  {
    // Usage:
    //   ctkDICOMRetrieve StudyUID OutputDirectory callingAETitle calledAETitle host calledPort moveDestinationAETitle

    CHECK_EXIT_SUCCESS(
          run_process(out, "client", WaitForFinished, ctkDICOMRetrieve, ctkDICOMRetrieve_exe,
                      {
                        studyUID,
                        ctkDICOMRetrieve_directory,
                        "CTK_AE",
                        "CTK_AE",
                        "localhost",
                        "11112",
                        "CTK_CLIENT_AE"
                      }));
  }


  //
  // clients are finished, now kill server and print output
  //
  CHECK_EXIT_SUCCESS(
        wait_for_finished(out, "server", dcmqrscp, /*kill=*/true));

  return EXIT_SUCCESS;
}

