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

#include "ctkXnatProject.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"

class ctkXnatProjectPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatProjectPrivate(ctkXnatConnection* connection)
    : ctkXnatObjectPrivate(connection)
  {}

  void reset()
  {
    secondaryId.clear();
    piFirstName.clear();
    piLastName.clear();
    uri.clear();
  }

  QString secondaryId;
  QString piFirstName;
  QString piLastName;
  QString uri;
};

ctkXnatProject::ctkXnatProject(ctkXnatConnection* connection)
  : ctkXnatObject(*new ctkXnatProjectPrivate(connection))
{
}

ctkXnatProject::Pointer ctkXnatProject::Create(ctkXnatConnection* connection)
{
  Pointer project(new ctkXnatProject(connection));
  project->d_func()->selfPtr = project;
  return project;
}

ctkXnatProject::~ctkXnatProject()
{
}

const QString& ctkXnatProject::secondaryId() const
{
  Q_D(const ctkXnatProject);
  return d->secondaryId;
}

void ctkXnatProject::setSecondaryId(const QString& secondaryId)
{
  Q_D(ctkXnatProject);
  d->secondaryId = secondaryId;
}

const QString& ctkXnatProject::piFirstName() const
{
  Q_D(const ctkXnatProject);
  return d->piFirstName;
}

void ctkXnatProject::setPiFirstName(const QString& piFirstName)
{
  Q_D(ctkXnatProject);
  d->piFirstName = piFirstName;
}

const QString& ctkXnatProject::piLastName() const
{
  Q_D(const ctkXnatProject);
  return d->piLastName;
}

void ctkXnatProject::setPiLastName(const QString& piLastName)
{
  Q_D(ctkXnatProject);
  d->piLastName = piLastName;
}

const QString& ctkXnatProject::uri() const
{
  Q_D(const ctkXnatProject);
  return d->uri;
}

void ctkXnatProject::setUri(const QString& uri)
{
  Q_D(ctkXnatProject);
  d->uri = uri;
}

void ctkXnatProject::reset()
{
  Q_D(ctkXnatProject);
  ctkXnatObject::reset();
}

void ctkXnatProject::fetchImpl()
{
  Q_D(ctkXnatProject);
  ctkXnatObject::Pointer self = d->selfPtr;
  getConnection()->fetch(self.staticCast<ctkXnatProject>());
}

void ctkXnatProject::remove()
{
  //connection->remove(this);
}
