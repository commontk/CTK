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
  this->ConnectionName = "";
  this->CallingAETitle = "";
  this->CalledAETitle = "";
  this->Host = "";
  this->MoveDestinationAETitle = "";
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
CTK_SET_CPP(ctkDICOMServer, const QString&, setConnectionName, ConnectionName);
CTK_GET_CPP(ctkDICOMServer, QString, connectionName, ConnectionName)
CTK_SET_CPP(ctkDICOMServer, const bool&, setQueryRetrieveEnabled, QueryRetrieveEnabled);
CTK_GET_CPP(ctkDICOMServer, bool, queryRetrieveEnabled, QueryRetrieveEnabled)
CTK_SET_CPP(ctkDICOMServer, const bool&, setStorageEnabled, StorageEnabled);
CTK_GET_CPP(ctkDICOMServer, bool, storageEnabled, StorageEnabled)
CTK_SET_CPP(ctkDICOMServer, const bool&, setTrustedEnabled, TrustedEnabled);
CTK_GET_CPP(ctkDICOMServer, bool, trustedEnabled, TrustedEnabled)
CTK_SET_CPP(ctkDICOMServer, const QString&, setCallingAETitle, CallingAETitle);
CTK_GET_CPP(ctkDICOMServer, QString, callingAETitle, CallingAETitle)
CTK_SET_CPP(ctkDICOMServer, const QString&, setCalledAETitle, CalledAETitle);
CTK_GET_CPP(ctkDICOMServer, QString, calledAETitle, CalledAETitle)
CTK_SET_CPP(ctkDICOMServer, const QString&, setHost, Host);
CTK_GET_CPP(ctkDICOMServer, QString, host, Host)
CTK_SET_CPP(ctkDICOMServer, const int&, setPort, Port);
CTK_GET_CPP(ctkDICOMServer, int, port, Port)
CTK_SET_CPP(ctkDICOMServer, RetrieveProtocol, setRetrieveProtocol, RetrieveProtocol);
CTK_GET_CPP(ctkDICOMServer, ctkDICOMServer::RetrieveProtocol, retrieveProtocol, RetrieveProtocol)
CTK_SET_CPP(ctkDICOMServer, const QString&, setMoveDestinationAETitle, MoveDestinationAETitle);
CTK_GET_CPP(ctkDICOMServer, QString, moveDestinationAETitle, MoveDestinationAETitle)
CTK_SET_CPP(ctkDICOMServer, const bool&, setKeepAssociationOpen, KeepAssociationOpen);
CTK_GET_CPP(ctkDICOMServer, bool, keepAssociationOpen, KeepAssociationOpen)
CTK_SET_CPP(ctkDICOMServer, const int&, setConnectionTimeout, ConnectionTimeout);
CTK_GET_CPP(ctkDICOMServer, int, connectionTimeout, ConnectionTimeout)

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
