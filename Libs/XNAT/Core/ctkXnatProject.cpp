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

class ctkXnatProjectPrivate
{
public:
  QString id;
  QString secondaryId;
  QString name;
  QString description;
  QString piFirstName;
  QString piLastName;
  QString uri;
};

ctkXnatProject::ctkXnatProject(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatProjectPrivate())
{
}

ctkXnatProject::~ctkXnatProject()
{
}

const QString& ctkXnatProject::id() const
{
  Q_D(const ctkXnatProject);
  return d->id;
}

void ctkXnatProject::setId(const QString& id)
{
  Q_D(ctkXnatProject);
  d->id = id;
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

const QString& ctkXnatProject::name() const
{
  Q_D(const ctkXnatProject);
  return d->name;
}

void ctkXnatProject::setName(const QString& name)
{
  Q_D(ctkXnatProject);
  d->name = name;
}

const QString& ctkXnatProject::description() const
{
  Q_D(const ctkXnatProject);
  return d->description;
}

void ctkXnatProject::setDescription(const QString& description)
{
  Q_D(ctkXnatProject);
  d->description = description;
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

void ctkXnatProject::fetch(ctkXnatConnection* connection)
{
  return connection->fetch(this);
}

void ctkXnatProject::remove(ctkXnatConnection* connection)
{
  connection->remove(this);
}

QString ctkXnatProject::getKind() const
{
  return "subject";
}

bool ctkXnatProject::isModifiable(int /*parentIndex*/) const
{
  return true;
}

QString ctkXnatProject::getModifiableChildKind(int /*parentIndex*/) const
{
  return "project";
}
