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

#include "ctkXnatProject.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"

class ctkXnatSubjectPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatSubjectPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    insertDate.clear();
    insertUser.clear();
    uri.clear();
    projects.clear();
  }

  QString insertDate;
  QString insertUser;
  QString uri;

  QList<ctkXnatProject::WeakPointer> projects;
};

ctkXnatSubject::ctkXnatSubject()
: ctkXnatObject(*new ctkXnatSubjectPrivate())
{
}

ctkXnatSubject::~ctkXnatSubject()
{
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

void ctkXnatSubject::reset()
{
  Q_D(ctkXnatSubject);
  d->reset();
}

void ctkXnatSubject::fetchImpl()
{
  Q_D(ctkXnatSubject);
  ctkXnatObject::Pointer self = d->selfPtr;
  getConnection()->fetch(self.staticCast<ctkXnatSubject>());
}

ctkXnatSubject::Pointer ctkXnatSubject::Create()
{
  Pointer subject(new ctkXnatSubject());
  subject->d_func()->selfPtr = subject;
  return subject;
}
