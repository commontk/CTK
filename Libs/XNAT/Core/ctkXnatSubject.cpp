/*=============================================================================

  Library: XNAT/Core

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

#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatProject.h"
#include "ctkXnatSession.h"

#include <QDebug>

const QString ctkXnatSubject::PROJECT_ID = "project";
const QString ctkXnatSubject::DATE_OF_BIRTH = "dob";
const QString ctkXnatSubject::GENDER = "gender";
const QString ctkXnatSubject::HANDEDNESS = "handedness";
const QString ctkXnatSubject::WEIGHT = "weight";
const QString ctkXnatSubject::HEIGHT = "height";
const QString ctkXnatSubject::INSERT_DATE = "insert_date";
const QString ctkXnatSubject::INSERT_USER = "insert_user";

//----------------------------------------------------------------------------
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
//    uri.clear();
    projects.clear();
  }

  QString insertDate;
  QString insertUser;
//  QString uri;

  QList<ctkXnatProject*> projects;
};


//----------------------------------------------------------------------------
ctkXnatSubject::ctkXnatSubject(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatSubjectPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatSubject::~ctkXnatSubject()
{
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::name() const
{
  return this->label();
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setName(const QString &name)
{
  this->setLabel(name);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::label() const
{
  return this->property(LABEL);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setLabel(const QString &label)
{
  this->setProperty(LABEL, label);
}

//----------------------------------------------------------------------------
const QString& ctkXnatSubject::insertDate() const
{
  Q_D(const ctkXnatSubject);
  return d->insertDate;
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setInsertDate(const QString& insertDate)
{
  Q_D(ctkXnatSubject);
  d->insertDate = insertDate;
}

//----------------------------------------------------------------------------
const QString& ctkXnatSubject::insertUser() const
{
  Q_D(const ctkXnatSubject);
  return d->insertUser;
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setInsertUser(const QString& insertUser)
{
  Q_D(ctkXnatSubject);
  d->insertUser = insertUser;
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::projectID() const
{
  return this->property(PROJECT_ID);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setProjectID(const QString &projectID)
{
  this->setProperty(PROJECT_ID, projectID);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::dateOfBirth() const
{
  return this->property(DATE_OF_BIRTH);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setDateOfBirth(const QString &dateOfBirth)
{
  this->setProperty(DATE_OF_BIRTH, dateOfBirth);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::gender() const
{
  return this->property(GENDER);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setGender(const QString &gender)
{
  this->setProperty(GENDER, gender);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::handedness() const
{
  return this->property(HANDEDNESS);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setHandedness(const QString &handedness)
{
  this->setProperty(HANDEDNESS, handedness);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::height() const
{
  return this->property(HEIGHT);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setHeight(const QString &height)
{
  this->setProperty(HEIGHT, height);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::weight() const
{
  return this->property(WEIGHT);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::setWeight(const QString &weight)
{
  this->setProperty(WEIGHT, weight);
}

//----------------------------------------------------------------------------
QString ctkXnatSubject::resourceUri() const
{
  if (this->id().isEmpty())
  {
    return QString("%1/subjects/%2").arg(parent()->resourceUri(), this->label());
  }
  return QString("%1/subjects/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatSubject::reset()
{
  Q_D(ctkXnatSubject);
  d->reset();
}

//----------------------------------------------------------------------------
void ctkXnatSubject::fetchImpl()
{
  QList<ctkXnatObject*> experiments;
  experiments.append(this->fetchImageSessionData());
  experiments.append(this->fetchSubjectVariablesData());

  foreach (ctkXnatObject* experiment, experiments)
  {
    QString label = experiment->name();
    if (!label.isEmpty())
    {
      experiment->setId(label);
    }

    this->add(experiment);
  }
  this->fetchResources();
}

//----------------------------------------------------------------------------
QList<ctkXnatObject*> ctkXnatSubject::fetchImageSessionData()
{
  QString experimentsUri = this->resourceUri() + "/experiments";
  ctkXnatSession* const session = this->session();
  QMap<QString, QString> paramMap;
  QString arglist = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10")
    .arg(ctkXnatObject::ID)
    .arg(ctkXnatObject::LABEL)
    .arg(ctkXnatObject::XSI_SCHEMA_TYPE)
    .arg(INSERT_DATE)
    .arg(INSERT_USER)
    .arg(ctkXnatObject::URI)
    .arg(ctkXnatExperiment::DATE_OF_ACQUISITION)
    .arg(ctkXnatExperiment::TIME_OF_ACQUISITION)
    .arg(ctkXnatExperiment::SCANNER_TYPE)
    .arg(ctkXnatExperiment::IMAGE_MODALITY);
  paramMap.insert("columns", arglist);
  paramMap.insert(ctkXnatObject::XSI_SCHEMA_TYPE, ctkXnatDefaultSchemaTypes::XSI_IMAGE_SESSION_DATA);
  QUuid queryId = session->httpGet(experimentsUri, paramMap);
  return session->httpResults(queryId, ctkXnatDefaultSchemaTypes::XSI_EXPERIMENT);
}

//----------------------------------------------------------------------------
QList<ctkXnatObject*> ctkXnatSubject::fetchSubjectVariablesData()
{
  QString experimentsUri = this->resourceUri() + "/experiments";
  ctkXnatSession* const session = this->session();
  QMap<QString, QString> paramMap;
  QString arglist = QString("%1,%2,%3,%4,%5,%6")
    .arg(ctkXnatObject::ID)
    .arg(ctkXnatObject::LABEL)
    .arg(ctkXnatObject::XSI_SCHEMA_TYPE)
    .arg(INSERT_DATE)
    .arg(INSERT_USER)
    .arg(ctkXnatObject::URI);
  paramMap.insert("columns", arglist);
  paramMap.insert(ctkXnatObject::XSI_SCHEMA_TYPE, ctkXnatDefaultSchemaTypes::XSI_SUBJECT_VARIABLE_DATA);
  QUuid queryId = session->httpGet(experimentsUri, paramMap);
  return session->httpResults(queryId, ctkXnatDefaultSchemaTypes::XSI_EXPERIMENT);
}

//----------------------------------------------------------------------------
void ctkXnatSubject::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatSubject::downloadImpl(const QString& filename) not yet implemented or not available by REST API";
}
