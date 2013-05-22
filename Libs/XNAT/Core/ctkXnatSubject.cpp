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

#include "ctkXnatSubject.h"

#include "ctkXnatConnection.h"
#include "ctkXnatSubject.h"

class ctkXnatSubjectPrivate
{
public:
  QString id;
  QString project;
  QString label;
  QString insertDate;
  QString insertUser;
  QString uri;
};

ctkXnatSubject::ctkXnatSubject(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatSubjectPrivate())
{
}

ctkXnatSubject::~ctkXnatSubject()
{
}

const QString& ctkXnatSubject::id() const
{
  Q_D(const ctkXnatSubject);
  return d->id;
}

void ctkXnatSubject::setId(const QString& id)
{
  Q_D(ctkXnatSubject);
  d->id = id;
}

const QString& ctkXnatSubject::project() const
{
  Q_D(const ctkXnatSubject);
  return d->project;
}

void ctkXnatSubject::setProject(const QString& project)
{
  Q_D(ctkXnatSubject);
  d->project = project;
}

const QString& ctkXnatSubject::label() const
{
  Q_D(const ctkXnatSubject);
  return d->label;
}

void ctkXnatSubject::setLabel(const QString& label)
{
  Q_D(ctkXnatSubject);
  d->label = label;
}

const QString& ctkXnatSubject::insertDate() const
{
  Q_D(const ctkXnatSubject);
  return d->insertDate;
}

void ctkXnatSubject::setInsertDate(const QString& insertDate)
{
  Q_D(ctkXnatSubject);
  d->insertDate = insertDate;
}

const QString& ctkXnatSubject::insertUser() const
{
  Q_D(const ctkXnatSubject);
  return d->insertUser;
}

void ctkXnatSubject::setInsertUser(const QString& insertUser)
{
  Q_D(ctkXnatSubject);
  d->insertUser = insertUser;
}

const QString& ctkXnatSubject::uri() const
{
  Q_D(const ctkXnatSubject);
  return d->uri;
}

void ctkXnatSubject::setUri(const QString& uri)
{
  Q_D(ctkXnatSubject);
  d->uri = uri;
}

void ctkXnatSubject::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

QString ctkXnatSubject::getKind() const
{
  return "experiment";
}

bool ctkXnatSubject::isModifiable(int childIndex) const
{
  if (getChildren()[childIndex] == 0)
  {
    return false;
  }
  return getChildren()[childIndex]->isModifiable();
}
