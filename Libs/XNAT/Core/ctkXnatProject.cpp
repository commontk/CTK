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

#include "ctkXnatProject.h"

#include "ctkXnatDataModel.h"
#include "ctkXnatSession.h"
#include "ctkXnatSubject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"


//----------------------------------------------------------------------------
class ctkXnatProjectPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatProjectPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }

//  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatProject::ctkXnatProject(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatProjectPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatProject::~ctkXnatProject()
{
}

//----------------------------------------------------------------------------
QString ctkXnatProject::resourceUri() const
{
  return QString("%1/data/archive/projects/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
QString ctkXnatProject::childDataType() const
{
  return "Subjects";
}

//----------------------------------------------------------------------------
const QString& ctkXnatProject::secondaryId() const
{
  return property(ctkXnatObjectFields::SECONDARY_ID);
}

//----------------------------------------------------------------------------
void ctkXnatProject::setSecondaryId(const QString& secondaryId)
{
  setProperty(ctkXnatObjectFields::SECONDARY_ID, secondaryId);
}

//----------------------------------------------------------------------------
const QString& ctkXnatProject::piFirstName() const
{
  return property(ctkXnatObjectFields::PI_FIRSTNAME);
}

//----------------------------------------------------------------------------
void ctkXnatProject::setPiFirstName(const QString& piFirstName)
{
  setProperty(ctkXnatObjectFields::PI_FIRSTNAME, piFirstName);
}

//----------------------------------------------------------------------------
const QString& ctkXnatProject::piLastName() const
{
  return property(ctkXnatObjectFields::PI_LASTNAME);
}

//----------------------------------------------------------------------------
void ctkXnatProject::setPiLastName(const QString& piLastName)
{
  setProperty(ctkXnatObjectFields::PI_LASTNAME, piLastName);
}

//----------------------------------------------------------------------------
//const QString& ctkXnatProject::uri() const
//{
//  Q_D(const ctkXnatProject);
//  return d->uri;
//}

//----------------------------------------------------------------------------
//void ctkXnatProject::setUri(const QString& uri)
//{
//  Q_D(ctkXnatProject);
//  d->uri = uri;
//}

//----------------------------------------------------------------------------
void ctkXnatProject::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatProject::fetchImpl()
{
  QString subjectsUri = this->resourceUri() + "/subjects";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(subjectsUri);
  QList<ctkXnatObject*> subjects = session->httpResults(queryId,
                                                        ctkXnatDefaultSchemaTypes::XSI_SUBJECT);

  foreach (ctkXnatObject* subject, subjects)
  {
    QString label = subject->property(ctkXnatObjectFields::LABEL);
    if (!label.isEmpty())
    {
      subject->setProperty(ctkXnatObjectFields::ID, label);
    }

    this->add(subject);
  }
  this->fetchResources();
}
