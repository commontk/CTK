/*=============================================================================

  Plugin: org.commontk.xnat

  Copyright (c) University College London,
    Centre for Medical Image Computing

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkXnatSessionFactory.h"

#include "ctkXnatSession.h"
#include "ctkXnatObject.h"
#include "ctkXnatDataModel.h"

#include <QDebug>

// ctkXnatSessionFactory class

ctkXnatSession* ctkXnatSessionFactory::makeConnection(const QString& url, const QString& user, const QString& password)
{
  // create XNAT connection
  ctkXnatSession* session = new ctkXnatSession;

  // test XNAT connection
  try
  {
    testConnection(session);
  }
  catch (...)
  {
    delete session;
    throw;
  }

  session->setUrl(url);
  qDebug() << "ctkXnatSessionFactory::makeConnection(const QString& url, const QString& user, const QString& password) url:" << url;
  session->setUserName(user);
  session->setPassword(password);

  // return XNAT connection
  return session;
}

void ctkXnatSessionFactory::testConnection(ctkXnatSession* session)
{
  // test connection by retrieving project names from XNAT
  session->dataModel();

  // TODO E.g. get version
}
