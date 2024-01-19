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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QSharedPointer>
#include <QString>

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMServer.h"

static ctkLogger logger("org.commontk.dicom.DICOMServer");

//------------------------------------------------------------------------------
class ctkDICOMServerPrivate : public QObject
{
  Q_DECLARE_PUBLIC(ctkDICOMServer);

protected:
  ctkDICOMServer* const q_ptr;

public:
  ctkDICOMServerPrivate(ctkDICOMServer& obj);
  ~ctkDICOMServerPrivate() = default;

  QString ConnectionName;
  bool QueryRetrieveEnabled;
  bool StorageEnabled;
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  int Port;
  ctkDICOMServer::RetrieveProtocol RetrieveProtocol;
  bool KeepAssociationOpen;
  QString MoveDestinationAETitle;
  int ConnectionTimeout;
  QSharedPointer<ctkDICOMServer> ProxyServer;
};

//------------------------------------------------------------------------------
// ctkDICOMServerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMServerPrivate::ctkDICOMServerPrivate(ctkDICOMServer& obj)
  : q_ptr(&obj)
{
  this->ConnectionName = "";
  this->CallingAETitle = "";
  this->CalledAETitle = "";
  this->Host = "";
  this->MoveDestinationAETitle = "";
  this->QueryRetrieveEnabled = true;
  this->StorageEnabled = true;
  this->KeepAssociationOpen = false;
  this->ConnectionTimeout = 10;
  this->Port = 80;
  this->RetrieveProtocol = ctkDICOMServer::RetrieveProtocol::CGET;
  this->ProxyServer = nullptr;
}

//------------------------------------------------------------------------------
// ctkDICOMServer methods

//------------------------------------------------------------------------------
ctkDICOMServer::ctkDICOMServer(QObject* parent)
  : QObject(parent),
    d_ptr(new ctkDICOMServerPrivate(*this))
{
}

//------------------------------------------------------------------------------
ctkDICOMServer::~ctkDICOMServer() = default;

//------------------------------------------------------------------------------
void ctkDICOMServer::setConnectionName(const QString& connectionName)
{
  Q_D(ctkDICOMServer);
  d->ConnectionName = connectionName;
}

//------------------------------------------------------------------------------
QString ctkDICOMServer::connectionName() const
{
  Q_D(const ctkDICOMServer);
  return d->ConnectionName;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setQueryRetrieveEnabled(bool queryRetrieveEnabled)
{
  Q_D(ctkDICOMServer);
  d->QueryRetrieveEnabled = queryRetrieveEnabled;
}

//------------------------------------------------------------------------------
bool ctkDICOMServer::queryRetrieveEnabled() const
{
  Q_D(const ctkDICOMServer);
  return d->QueryRetrieveEnabled;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setStorageEnabled(bool storageEnabled)
{
  Q_D(ctkDICOMServer);
  d->StorageEnabled = storageEnabled;
}

//------------------------------------------------------------------------------
bool ctkDICOMServer::storageEnabled() const
{
  Q_D(const ctkDICOMServer);
  return d->StorageEnabled;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setCallingAETitle(const QString& callingAETitle)
{
  Q_D(ctkDICOMServer);
  d->CallingAETitle = callingAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMServer::callingAETitle() const
{
  Q_D(const ctkDICOMServer);
  return d->CallingAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setCalledAETitle(const QString& calledAETitle)
{
  Q_D(ctkDICOMServer);
  d->CalledAETitle = calledAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMServer::calledAETitle() const
{
  Q_D(const ctkDICOMServer);
  return d->CalledAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setHost(const QString& host)
{
  Q_D(ctkDICOMServer);
  d->Host = host;
}

//------------------------------------------------------------------------------
QString ctkDICOMServer::host() const
{
  Q_D(const ctkDICOMServer);
  return d->Host;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setPort(int port)
{
  Q_D(ctkDICOMServer);
  d->Port = port;
}

//------------------------------------------------------------------------------
int ctkDICOMServer::port() const
{
  Q_D(const ctkDICOMServer);
  return d->Port;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setRetrieveProtocol(RetrieveProtocol protocol)
{
  Q_D(ctkDICOMServer);
  d->RetrieveProtocol = protocol;
}

//------------------------------------------------------------------------------
ctkDICOMServer::RetrieveProtocol ctkDICOMServer::retrieveProtocol() const
{
  Q_D(const ctkDICOMServer);
  return d->RetrieveProtocol;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setRetrieveProtocolAsString(const QString& protocolString)
{
  Q_D(ctkDICOMServer);

  if (protocolString == "CGET")
    {
    d->RetrieveProtocol = RetrieveProtocol::CGET;
    }
  else if (protocolString == "CMOVE")
    {
    d->RetrieveProtocol = RetrieveProtocol::CMOVE;
    }
  /*else if (protocolString == "WADO") To Do
    {
    d->RetrieveProtocol = RetrieveProtocol::WADO;
    }*/
}

//------------------------------------------------------------------------------
QString ctkDICOMServer::retrieveProtocolAsString() const
{
  Q_D(const ctkDICOMServer);

  QString protocolString = "";
  switch (d->RetrieveProtocol)
    {
    case RetrieveProtocol::CGET:
      protocolString = "CGET";
      break;
    case RetrieveProtocol::CMOVE:
      protocolString = "CMOVE";
      break;
    /*case RetrieveProtocol::WADO: To Do
      protocolString = "WADO";
      break; */
    default:
      break;
    }

  return protocolString;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setMoveDestinationAETitle(const QString& moveDestinationAETitle)
{
  Q_D(ctkDICOMServer);
  if (moveDestinationAETitle != d->MoveDestinationAETitle)
  {
    d->MoveDestinationAETitle = moveDestinationAETitle;
  }
}
//------------------------------------------------------------------------------
QString ctkDICOMServer::moveDestinationAETitle() const
{
  Q_D(const ctkDICOMServer);
  return d->MoveDestinationAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setKeepAssociationOpen(bool keepOpen)
{
  Q_D(ctkDICOMServer);
  d->KeepAssociationOpen = keepOpen;
}

//------------------------------------------------------------------------------
bool ctkDICOMServer::keepAssociationOpen()
{
  Q_D(const ctkDICOMServer);
  return d->KeepAssociationOpen;
}

//-----------------------------------------------------------------------------
void ctkDICOMServer::setConnectionTimeout(int timeout)
{
  Q_D(ctkDICOMServer);
  d->ConnectionTimeout = timeout;
}

//-----------------------------------------------------------------------------
int ctkDICOMServer::connectionTimeout()
{
  Q_D(const ctkDICOMServer);
  return d->ConnectionTimeout;
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMServer::proxyServer() const
{
  Q_D(const ctkDICOMServer);
  return d->ProxyServer.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMServer> ctkDICOMServer::proxyServerShared() const
{
  Q_D(const ctkDICOMServer);
  return d->ProxyServer;
}

//----------------------------------------------------------------------------
void ctkDICOMServer::setProxyServer(ctkDICOMServer& proxyServer)
{
  Q_D(ctkDICOMServer);
  d->ProxyServer = QSharedPointer<ctkDICOMServer>(&proxyServer, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMServer::setProxyServer(QSharedPointer<ctkDICOMServer> proxyServer)
{
  Q_D(ctkDICOMServer);
  d->ProxyServer = proxyServer;
}
