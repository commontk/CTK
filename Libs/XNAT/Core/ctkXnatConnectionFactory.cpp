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

#include "ctkXnatConnectionFactory.h"

#include "ctkXnatConnection.h"
#include "ctkXnatException.h"
#include "ctkXnatObject.h"
#include "ctkXnatServer.h"

// ctkXnatConnectionFactory class

ctkXnatConnection* ctkXnatConnectionFactory::makeConnection(const QString& url, const QString& user, const QString& password)
{
  // create XNAT connection
  ctkXnatConnection* connection = new ctkXnatConnection;

  // test XNAT connection
  try
  {
    testConnection(connection);
  }
  catch (ctkXnatException& e)
  {
    delete connection;
    throw;
  }

  connection->setUrl(url);
  connection->setUserName(user);
  connection->setPassword(password);

  // return XNAT connection
  return connection;
}

void ctkXnatConnectionFactory::testConnection(ctkXnatConnection* connection)
{
  // test connection by retrieving project names from XNAT
  ctkXnatServer::Pointer server = connection->server();

  // TODO E.g. get version
}
