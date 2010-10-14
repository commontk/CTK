
// Qt includes
#include <QTextStream>
#include <QCoreApplication>
#include <QProcess>

// STD includes
#include <iostream>
#include <cstdlib>

  /*
/pieper/ctk/latest/CTK-superbuild/CMakeExternals/Install/bin/dcmqrscp -c /pieper/ctk/latest/CTK-superbuild/CTK-build/Testing/Temporary/dcmqrscp.cfg -d -v

storescu -aec COMMONTK -aet CTK_AE localhost 11112 /data/pieper-face-2005-05-11/1.2.840.113619.2.135.3596.6358736.5118.1115807980.182.UID/000001.SER/000001.IMA

findscu -aet CTK_AE -aec COMMONTK -P -k 0010,0010=\* localhost 11112 patqry.dcm

./CTK-build/bin/ctkDICOMQuery test.db CTK_AE COMMONTK localhost 11112


./CTK-build/bin/ctkDICOMRetrieve 1.2.840.113619.2.135.3596.6358736.5118.1115807980.182  /tmp/hoot CTK_AE 11113 CTK_AE localhost 11112 CTK_CLIENT_AE
*/

int ctkDICOMApplicationTest1(int argc, char * argv []) {
  
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
  // now push some dicom data in using storescp
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

