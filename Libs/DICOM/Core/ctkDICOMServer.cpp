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
  ~ctkDICOMServerPrivate();

  QString ConnectionName;
  bool QueryRetrieveEnabled;
  bool StorageEnabled;
  bool TrustedEnabled;
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  int Port;
  ctkDICOMServer::RetrieveProtocol RetrieveProtocol;
  bool KeepAssociationOpen;
  QString MoveDestinationAETitle;
  int ConnectionTimeout;
  ctkDICOMServer* ProxyServer;
};

//------------------------------------------------------------------------------
// ctkDICOMServerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMServerPrivate::ctkDICOMServerPrivate(ctkDICOMServer& obj)
  : q_ptr(&obj)
{
  this->QueryRetrieveEnabled = true;
  this->StorageEnabled = true;
  this->TrustedEnabled = true;
  this->KeepAssociationOpen = false;
  this->ConnectionTimeout = 10;
  this->Port = 80;
  this->RetrieveProtocol = ctkDICOMServer::RetrieveProtocol::CGET;
  this->ProxyServer = nullptr;
}

//------------------------------------------------------------------------------
ctkDICOMServerPrivate::~ctkDICOMServerPrivate()
{
  if (this->ProxyServer)
    {
    delete this->ProxyServer;
    this->ProxyServer = nullptr;
    }
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
CTK_GET_CPP(ctkDICOMServer, QString, connectionName, ConnectionName)
CTK_GET_CPP(ctkDICOMServer, bool, queryRetrieveEnabled, QueryRetrieveEnabled)
CTK_GET_CPP(ctkDICOMServer, bool, storageEnabled, StorageEnabled)
CTK_GET_CPP(ctkDICOMServer, bool, trustedEnabled, TrustedEnabled)
CTK_GET_CPP(ctkDICOMServer, QString, callingAETitle, CallingAETitle)
CTK_GET_CPP(ctkDICOMServer, QString, calledAETitle, CalledAETitle)
CTK_GET_CPP(ctkDICOMServer, QString, host, Host)
CTK_GET_CPP(ctkDICOMServer, int, port, Port)
CTK_GET_CPP(ctkDICOMServer, ctkDICOMServer::RetrieveProtocol, retrieveProtocol, RetrieveProtocol)
CTK_GET_CPP(ctkDICOMServer, QString, moveDestinationAETitle, MoveDestinationAETitle)
CTK_GET_CPP(ctkDICOMServer, bool, keepAssociationOpen, KeepAssociationOpen)
CTK_GET_CPP(ctkDICOMServer, int, connectionTimeout, ConnectionTimeout)

//------------------------------------------------------------------------------
void ctkDICOMServer::setConnectionName(const QString& connectionName)
{
  Q_D(ctkDICOMServer);
  d->ConnectionName = connectionName;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setQueryRetrieveEnabled(const bool& queryRetrieveEnabled)
{
  Q_D(ctkDICOMServer);
  d->QueryRetrieveEnabled = queryRetrieveEnabled;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setStorageEnabled(const bool& storageEnabled)
{
  Q_D(ctkDICOMServer);
  d->StorageEnabled = storageEnabled;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setTrustedEnabled(const bool& trustedEnabled)
{
  Q_D(ctkDICOMServer);
  d->TrustedEnabled = trustedEnabled;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setCallingAETitle(const QString& callingAETitle)
{
  Q_D(ctkDICOMServer);
  d->CallingAETitle = callingAETitle;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setCalledAETitle(const QString& calledAETitle)
{
  Q_D(ctkDICOMServer);
  d->CalledAETitle = calledAETitle;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setHost(const QString& host)
{
  Q_D(ctkDICOMServer);
  d->Host = host;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setPort(const int& port)
{
  Q_D(ctkDICOMServer);
  d->Port = port;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setRetrieveProtocol(RetrieveProtocol protocol)
{
  Q_D(ctkDICOMServer);
  d->RetrieveProtocol = protocol;
  emit serverModified(d->ConnectionName);
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
  emit serverModified(d->ConnectionName);
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
  d->MoveDestinationAETitle = moveDestinationAETitle;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setKeepAssociationOpen(const bool& keepAssociationOpen)
{
  Q_D(ctkDICOMServer);
  d->KeepAssociationOpen = keepAssociationOpen;
  emit serverModified(d->ConnectionName);
}

//------------------------------------------------------------------------------
void ctkDICOMServer::setConnectionTimeout(const int& timeout)
{
  Q_D(ctkDICOMServer);
  d->ConnectionTimeout = timeout;
  emit serverModified(d->ConnectionName);
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMServer::proxyServer() const
{
  Q_D(const ctkDICOMServer);
  return d->ProxyServer;
}

//----------------------------------------------------------------------------
void ctkDICOMServer::setProxyServer(const ctkDICOMServer& proxyServer)
{
  Q_D(ctkDICOMServer);
  d->ProxyServer = proxyServer.clone();
  emit serverModified(d->ConnectionName);
}

//----------------------------------------------------------------------------
ctkDICOMServer *ctkDICOMServer::clone() const
{
  ctkDICOMServer* newServer = new ctkDICOMServer;
  newServer->setConnectionName(this->connectionName());
  newServer->setQueryRetrieveEnabled(this->queryRetrieveEnabled());
  newServer->setStorageEnabled(this->storageEnabled());
  newServer->setTrustedEnabled(this->trustedEnabled());
  newServer->setCallingAETitle(this->callingAETitle());
  newServer->setCalledAETitle(this->calledAETitle());
  newServer->setHost(this->host());
  newServer->setPort(this->port());
  newServer->setRetrieveProtocol(this->retrieveProtocol());
  newServer->setMoveDestinationAETitle(this->moveDestinationAETitle());
  newServer->setKeepAssociationOpen(this->keepAssociationOpen());
  newServer->setConnectionTimeout(this->connectionTimeout());
  if (this->proxyServer())
    {
    newServer->setProxyServer(*this->proxyServer());
    }

  return newServer;
}
