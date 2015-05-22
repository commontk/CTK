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

#include "ctkXnatResourceFolder.h"

#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatResource.h"
#include "ctkXnatSession.h"

//----------------------------------------------------------------------------
class ctkXnatResourceFolderPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatResourceFolderPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
  }

};


//----------------------------------------------------------------------------
ctkXnatResourceFolder::ctkXnatResourceFolder(ctkXnatObject* parent)
  : ctkXnatObject(*new ctkXnatResourceFolderPrivate(), parent, QString::null)
{
  this->setId("resources");
  this->setProperty(LABEL, "Resources");
}

//----------------------------------------------------------------------------
ctkXnatResourceFolder::~ctkXnatResourceFolder()
{
}

//----------------------------------------------------------------------------
QString ctkXnatResourceFolder::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
QString ctkXnatResourceFolder::name() const
{
  return this->label();
}

//----------------------------------------------------------------------------
QString ctkXnatResourceFolder::label() const
{
  return this->property(LABEL);
}

//----------------------------------------------------------------------------
void ctkXnatResourceFolder::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatResourceFolder::fetchImpl()
{
  QString query = this->resourceUri();
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(query);

  QList<ctkXnatObject*> resources = session->httpResults(queryId,
                                                           ctkXnatDefaultSchemaTypes::XSI_RESOURCE);

  foreach (ctkXnatObject* resource, resources)
  {
    QString label = resource->name();
    if (label.isEmpty())
    {
      label = "NO NAME";
    }

    resource->setName(label);
    this->add(resource);
  }
}

//----------------------------------------------------------------------------
void ctkXnatResourceFolder::downloadImpl(const QString& filename)
{
  QString query = this->resourceUri() + "/ALL/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}

//----------------------------------------------------------------------------
void ctkXnatResourceFolder::saveImpl(bool /*overwrite*/)
{
  // Not implemented since a resource folder is automatically created when
  // a resource is uploaded
}
