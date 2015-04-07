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

#include "ctkXnatDataModel.h"

#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSession.h"
#include "ctkXnatProject.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <QDebug>

// --------------------------------------------------------------------------
class ctkXnatDataModelPrivate : public ctkXnatObjectPrivate
{
  explicit ctkXnatDataModelPrivate(ctkXnatSession* session);
  virtual ~ctkXnatDataModelPrivate();

private:
  friend class ctkXnatDataModel;

  ctkXnatSession* session;
};

// --------------------------------------------------------------------------
ctkXnatDataModelPrivate::ctkXnatDataModelPrivate(ctkXnatSession* connection)
: ctkXnatObjectPrivate()
, session(connection)
{
}

// --------------------------------------------------------------------------
ctkXnatDataModelPrivate::~ctkXnatDataModelPrivate()
{
}

// --------------------------------------------------------------------------
ctkXnatDataModel::ctkXnatDataModel(ctkXnatSession* session)
: ctkXnatObject(*new ctkXnatDataModelPrivate(session))
{
}

// --------------------------------------------------------------------------
QList<ctkXnatProject*> ctkXnatDataModel::projects() const
{
  QList<ctkXnatProject*> result;
  foreach(ctkXnatObject* obj, this->children())
  {
    result.push_back(static_cast<ctkXnatProject*>(obj));
  }
  return result;
}

// --------------------------------------------------------------------------
QString ctkXnatDataModel::resourceUri() const
{
  return "";
}

// --------------------------------------------------------------------------
QString ctkXnatDataModel::childDataType() const
{
  return "Projects";
}

// --------------------------------------------------------------------------
ctkXnatSession* ctkXnatDataModel::session() const
{
  Q_D(const ctkXnatDataModel);
  return d->session;
}

// --------------------------------------------------------------------------
void ctkXnatDataModel::fetchImpl()
{
  Q_D(ctkXnatDataModel);

  QString projectsUri("/data/archive/projects");

  QUuid queryId = d->session->httpGet(projectsUri);
  QList<ctkXnatObject*> projects = d->session->httpResults(queryId,
                                                           ctkXnatDefaultSchemaTypes::XSI_PROJECT);

  qDebug() << "ctkXnatDataModel::fetchImpl(): project number:" << projects.size();

  foreach (ctkXnatObject* project, projects)
  {
    this->add(project);
  }
}

//----------------------------------------------------------------------------
void ctkXnatDataModel::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatDataModel::downloadImpl(const QString& filename) not yet implemented or not available by REST API";
}
