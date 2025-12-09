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
#include <QMutex>
#include <QSettings>
#include <QString>
#include <QStringList>

// ctkDICOMCore includes
#include "ctkDICOMEcho.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/ofstd/ofstd.h> /* for class OFStandard */
#include <dcmtk/ofstd/ofstring.h>

//------------------------------------------------------------------------------
// Using dcmtk root log4cplus logger instead of ctkLogger because with ctkDICOMJobsAppender (dcmtk::log4cplus::Appender),
// logging is filtered by threadID and reported in the GUI per job.
dcmtk::log4cplus::Logger rootLogEcho = dcmtk::log4cplus::Logger::getRoot();

//------------------------------------------------------------------------------
class ctkDICOMEchoPrivate
{
public:
  ctkDICOMEchoPrivate();
  ~ctkDICOMEchoPrivate();

  /// \warning: releaseAssociation is not a thread safe method.
  /// If called concurrently from different threads DCMTK can crash.
  /// Therefore use this method instead of calling directly SCU->releaseAssociation()
  OFCondition releaseAssociation();

  QString ConnectionName;
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  QString JobUID;
  int Port;
  T_ASC_PresentationContextID PresentationContext;
  DcmSCU *SCU;
  bool Canceled;
  bool AssociationClosing;
  QMutex AssociationMutex;
};

//------------------------------------------------------------------------------
// ctkDICOMEchoPrivate methods

//------------------------------------------------------------------------------
ctkDICOMEchoPrivate::ctkDICOMEchoPrivate()
{
  this->Port = 80;

  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

  this->PresentationContext = 0;
  this->SCU = new DcmSCU();
  this->SCU->setACSETimeout(3);
  this->SCU->setConnectionTimeout(3);
  this->SCU->addPresentationContext(
    UID_VerificationSOPClass, transferSyntaxes);

  this->Canceled = false;
  this->AssociationClosing = false;
}

//------------------------------------------------------------------------------
ctkDICOMEchoPrivate::~ctkDICOMEchoPrivate()
{
  if (this->SCU && this->SCU->isConnected())
  {
    this->releaseAssociation();
  }

  if (this->SCU)
  {
    delete this->SCU;
  }
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMEchoPrivate::releaseAssociation()
{
  OFCondition status = EC_IllegalCall;
  if (!this->SCU)
    {
    return status;
    }

  QMutexLocker locker(&this->AssociationMutex);
  if (this->AssociationClosing)
  {
    return status;
  }

  this->AssociationClosing = true;
  status = this->SCU->releaseAssociation();
  this->AssociationClosing = false;

  return status;
}

//------------------------------------------------------------------------------
// ctkDICOMEcho methods

//------------------------------------------------------------------------------
ctkDICOMEcho::ctkDICOMEcho(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMEchoPrivate)
{
  Q_D(ctkDICOMEcho);

  d->SCU->setVerbosePCMode(false);
}

//------------------------------------------------------------------------------
ctkDICOMEcho::~ctkDICOMEcho() = default;

//------------------------------------------------------------------------------
CTK_SET_CPP_EMIT(ctkDICOMEcho, const QString&, setConnectionName, ConnectionName, connectionNameChanged);
CTK_GET_CPP(ctkDICOMEcho, QString, connectionName, ConnectionName)
CTK_SET_CPP_EMIT(ctkDICOMEcho, const QString&, setCallingAETitle, CallingAETitle, callingAETitleChanged);
CTK_GET_CPP(ctkDICOMEcho, QString, callingAETitle, CallingAETitle)
CTK_SET_CPP_EMIT(ctkDICOMEcho, const QString&, setCalledAETitle, CalledAETitle,calledAETitleChanged);
CTK_GET_CPP(ctkDICOMEcho, QString, calledAETitle, CalledAETitle)
CTK_SET_CPP_EMIT(ctkDICOMEcho, const QString&, setHost, Host,hostChanged);
CTK_GET_CPP(ctkDICOMEcho, QString, host, Host)
CTK_SET_CPP_EMIT(ctkDICOMEcho, const int&, setPort, Port, portChanged);
CTK_GET_CPP(ctkDICOMEcho, int, port, Port)
CTK_SET_CPP_EMIT(ctkDICOMEcho, const QString&, setJobUID, JobUID, jobUIDChanged);
CTK_GET_CPP(ctkDICOMEcho, QString, jobUID, JobUID)

//-----------------------------------------------------------------------------
void ctkDICOMEcho::setConnectionTimeout(const int& timeout)
{
  Q_D(ctkDICOMEcho);
  d->SCU->setACSETimeout(timeout);
  d->SCU->setConnectionTimeout(timeout);
  emit connectionTimeoutChanged(timeout);
}

//-----------------------------------------------------------------------------
int ctkDICOMEcho::connectionTimeout() const
{
  Q_D(const ctkDICOMEcho);
  return d->SCU->getConnectionTimeout();
}

//------------------------------------------------------------------------------
bool ctkDICOMEcho::wasCanceled()
{
  Q_D(const ctkDICOMEcho);
  return d->Canceled;
}

//------------------------------------------------------------------------------
bool ctkDICOMEcho::echo()
{
  Q_D(ctkDICOMEcho);

  d->SCU->setPeerAETitle(OFString(this->calledAETitle().toStdString().c_str()));
  d->SCU->setPeerHostName(OFString(this->host().toStdString().c_str()));
  d->SCU->setPeerPort(this->port());

  if (!d->SCU->initNetwork().good())
  {
    QString error = "Error initializing the network";
    DCMTK_LOG4CPLUS_ERROR_STR(rootLogEcho, error.toStdString().c_str());
    return false;
  }
  QString debug = "Negotiating Association";
  DCMTK_LOG4CPLUS_DEBUG_STR(rootLogEcho, debug.toStdString().c_str());

  OFCondition result = d->SCU->negotiateAssociation();
  if (result.bad())
  {
    QString error = "Error negotiating the association: " + QString(result.text());
    DCMTK_LOG4CPLUS_ERROR_STR(rootLogEcho, error.toStdString().c_str());
    return false;
  }

  d->PresentationContext = d->SCU->findPresentationContextID(
    UID_VerificationSOPClass,
    "" /* don't care about transfer syntax */);
  if (d->PresentationContext == 0)
  {
    QString error = "ECHO Request failed: No valid verification Presentation Context available";
    DCMTK_LOG4CPLUS_ERROR_STR(rootLogEcho, error.toStdString().c_str());
    d->releaseAssociation();
    return false;
  }

  QString info = "Seding Echo";
  DCMTK_LOG4CPLUS_INFO_STR(rootLogEcho, info.toStdString().c_str());
  // Issue ECHO request and let scu find presentation context itself (0)
  OFCondition status = d->SCU->sendECHORequest(d->PresentationContext);
  if (!status.good())
  {
    QString error = "Echo failed";
    DCMTK_LOG4CPLUS_ERROR_STR(rootLogEcho, error.toStdString().c_str());
    d->releaseAssociation();
    return false;
  }

  d->releaseAssociation();

  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMEcho::cancel()
{
  Q_D(ctkDICOMEcho);
  d->Canceled = true;
}
