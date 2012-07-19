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
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

// ctkDICOM includes
#include "ctkDICOMTester.h"
#include "ctkLogger.h"

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMTester" );
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ctkDICOMTesterPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMTester);
protected:
  ctkDICOMTester* const q_ptr;

public:
  ctkDICOMTesterPrivate(ctkDICOMTester&);
  ~ctkDICOMTesterPrivate();

  QString findFile(const QStringList& nameFilters, const QString& subDir)const;
  QString findDCMQRSCPExecutable()const;
  QString findDCMQRSCPConfigFile()const;
  QString findStoreSCUExecutable()const;
  QString findStoreSCPExecutable()const;
  void printProcessOutputs(const QString& program, QProcess* process)const;
  
  QProcess*   DCMQRSCPProcess;
  QProcess*   STORESCPProcess;
  QString     DCMQRSCPExecutable;
  QString     DCMQRSCPConfigFile;
  int         DCMQRSCPPort;
  int         STORESCPPort;
  QString     StoreSCUExecutable;
  QString     StoreSCPExecutable;
};

//------------------------------------------------------------------------------
// ctkDICOMTesterPrivate methods

//------------------------------------------------------------------------------
ctkDICOMTesterPrivate::ctkDICOMTesterPrivate(ctkDICOMTester& o): q_ptr(&o)
{
  Q_Q(ctkDICOMTester);

  this->DCMQRSCPProcess = 0;
  this->DCMQRSCPExecutable = this->findDCMQRSCPExecutable();
  this->DCMQRSCPConfigFile = this->findDCMQRSCPConfigFile();
  this->DCMQRSCPPort = 11112;
  this->STORESCPPort = 11113;
  this->StoreSCUExecutable = this->findStoreSCUExecutable();
  this->StoreSCPExecutable = this->findStoreSCPExecutable();

  // Start the storescp process and keep it active as long as this
  // class exists
  this->STORESCPProcess = new QProcess(q);
  // usage of storescp:
  //  storescp 11113
  QStringList storescpArgs;
  storescpArgs << QString::number(this->STORESCPPort);
  
  this->STORESCPProcess->start(this->StoreSCPExecutable, storescpArgs);
}

//------------------------------------------------------------------------------
ctkDICOMTesterPrivate::~ctkDICOMTesterPrivate()
{
  this->STORESCPProcess->terminate();
  if (!this->STORESCPProcess->waitForFinished())
    {
    this->STORESCPProcess->kill();
    }
  this->STORESCPProcess = 0;

  delete this->STORESCPProcess;
  if (this->DCMQRSCPProcess)
    {
    delete this->DCMQRSCPProcess;
    }
  this->DCMQRSCPProcess = 0;
}

//------------------------------------------------------------------------------
QString ctkDICOMTesterPrivate::findFile(const QStringList& nameFilters, const QString& subDir)const
{
  // Search in the build tree first
  QDir searchDir = QDir::current();
  do
    {
    QFileInfo dcmExecutables(searchDir, subDir);
    if (dcmExecutables.isDir() &&
        dcmExecutables.exists())
      {
      QDir bin(dcmExecutables.absoluteFilePath());
      QFileInfoList found = bin.entryInfoList(nameFilters, QDir::Files);
      if (found.count() > 0)
        {
        return found[0].absoluteFilePath();
        }
      }
    }
  while (searchDir.cdUp())
    ;
  // TODO: take care of the installed case
  return QString();
}

//------------------------------------------------------------------------------
QString ctkDICOMTesterPrivate::findDCMQRSCPExecutable()const
{
  return this->findFile(QStringList("dcmqrscp*"), "CMakeExternals/Install/bin");  
}

//------------------------------------------------------------------------------
QString ctkDICOMTesterPrivate::findDCMQRSCPConfigFile()const
{
  return this->findFile(QStringList("dcmqrscp.cfg"), "Libs/DICOM/Core");
}

//------------------------------------------------------------------------------
QString ctkDICOMTesterPrivate::findStoreSCUExecutable()const
{
  return this->findFile(QStringList("storescu*"), "CMakeExternals/Install/bin");  
}

//------------------------------------------------------------------------------
QString ctkDICOMTesterPrivate::findStoreSCPExecutable()const
{
  return this->findFile(QStringList("storescp*"), "CMakeExternals/Install/bin");  
}

//------------------------------------------------------------------------------
void ctkDICOMTesterPrivate::printProcessOutputs(const QString& program, QProcess* process)const
{
  QTextStream out(stdout);
  out << "Process " << program << " is finished.\n";

  QByteArray standardOutput = process->readAllStandardOutput();
  if (standardOutput.count())
    {  
    out << "Standard Output:\n";
    out << standardOutput;
    }
  QByteArray standardError = process->readAllStandardError();
  if (standardError.count())
    {  
    out << "Standard Error:\n";
    out << standardError;
    }
}

//------------------------------------------------------------------------------
// ctkDICOMTester methods

//------------------------------------------------------------------------------
ctkDICOMTester::ctkDICOMTester(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMTesterPrivate(*this))
{
}

//------------------------------------------------------------------------------
ctkDICOMTester::ctkDICOMTester(const QString& dcmqrscp,
                               const QString& configFile,
                               QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMTesterPrivate(*this))
{
  this->setDCMQRSCPExecutable(dcmqrscp);
  this->setDCMQRSCPConfigFile(configFile);
}

//------------------------------------------------------------------------------
ctkDICOMTester::~ctkDICOMTester()
{
  // just in case
  this->stopDCMQRSCP();
}

//------------------------------------------------------------------------------
void ctkDICOMTester::setDCMQRSCPExecutable(const QString& dcmqrscp)
{
  Q_D(ctkDICOMTester);
  d->DCMQRSCPExecutable = dcmqrscp;
}

//------------------------------------------------------------------------------
QString ctkDICOMTester::dcmqrscpExecutable()const
{
  Q_D(const ctkDICOMTester);
  return d->DCMQRSCPExecutable;
}
  
//------------------------------------------------------------------------------
void ctkDICOMTester::setDCMQRSCPConfigFile(const QString& configFile)
{
  Q_D(ctkDICOMTester);
  d->DCMQRSCPConfigFile = configFile;
}

//------------------------------------------------------------------------------
QString ctkDICOMTester::dcmqrscpConfigFile()const
{
  Q_D(const ctkDICOMTester);
  return d->DCMQRSCPConfigFile;
}
//------------------------------------------------------------------------------
void ctkDICOMTester::setStoreSCUExecutable(const QString& storeSCU)
{
  Q_D(ctkDICOMTester);
  d->StoreSCUExecutable = storeSCU;
}

//------------------------------------------------------------------------------
QString ctkDICOMTester::storeSCUExecutable()const
{
  Q_D(const ctkDICOMTester);
  return d->StoreSCUExecutable;
}

//------------------------------------------------------------------------------
void ctkDICOMTester::setStoreSCPExecutable(const QString& storeSCP)
{
  Q_D(ctkDICOMTester);
  d->StoreSCPExecutable = storeSCP;
}

//------------------------------------------------------------------------------
QString ctkDICOMTester::storeSCPExecutable()const
{
  Q_D(const ctkDICOMTester);
  return d->StoreSCPExecutable;
}

//------------------------------------------------------------------------------
void ctkDICOMTester::setDCMQRSCPPort(int port)
{
  Q_D(ctkDICOMTester);
  d->DCMQRSCPPort = port;
}

//------------------------------------------------------------------------------
int ctkDICOMTester::dcmqrscpPort()const
{
  Q_D(const ctkDICOMTester);
  return d->DCMQRSCPPort;
}

//------------------------------------------------------------------------------
QProcess* ctkDICOMTester::startDCMQRSCP()
{
  Q_D(ctkDICOMTester);
  if (d->DCMQRSCPProcess)
    {
    return 0;
    }
  d->DCMQRSCPProcess = new QProcess(this);

  QStringList dcmqrscpArgs;
  if (!d->DCMQRSCPConfigFile.isEmpty())
    {
    dcmqrscpArgs << "--config" << d->DCMQRSCPConfigFile;
    }
  //dcmqrscp_args << "--debug" << "--verbose";
  dcmqrscpArgs << QString::number(d->DCMQRSCPPort);

  d->DCMQRSCPProcess->start(d->DCMQRSCPExecutable, dcmqrscpArgs);
  d->DCMQRSCPProcess->waitForStarted(-1);

  return d->DCMQRSCPProcess;
}

//------------------------------------------------------------------------------
bool ctkDICOMTester::stopDCMQRSCP()
{
  Q_D(ctkDICOMTester);
  if (!d->DCMQRSCPProcess)
    {
    return false;
    }

  d->DCMQRSCPProcess->kill();
  bool res = d->DCMQRSCPProcess->waitForFinished(-1);

  d->printProcessOutputs("DCMQRSCP", d->DCMQRSCPProcess);

  delete d->DCMQRSCPProcess;
  d->DCMQRSCPProcess = 0;
  return res;
}

//------------------------------------------------------------------------------
bool ctkDICOMTester::storeData(const QStringList& data)
{
  Q_D(ctkDICOMTester);

  if (data.count() == 0)
    {
    return true;
    }

  // There is no point of calling storescu if no-one is listening
  if (!d->DCMQRSCPProcess)
    {
    return false;
    }

  QProcess storeSCU(this);
  // usage of storescu:
  // storescu -aec CTK_AE -aet CTK_AE localhost 11112 ./CMakeExternals/Source/CTKData/Data/DICOM/MRHEAD/*.IMA
  QStringList storescuArgs;
  storescuArgs << "-aec" << "CTK_AE";
  storescuArgs << "-aet" << "CTK_AE";
  storescuArgs << "localhost" <<  QString::number(d->DCMQRSCPPort);
  storescuArgs << data;
  
  storeSCU.start(d->StoreSCUExecutable, storescuArgs);
  bool res = storeSCU.waitForFinished(-1);

  d->printProcessOutputs("StoreSCU", &storeSCU);
  return res;
}
