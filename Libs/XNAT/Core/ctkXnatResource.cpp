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

#include "ctkXnatResource.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"

//----------------------------------------------------------------------------
class ctkXnatResourcePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatResourcePrivate()
  : ctkXnatObjectPrivate()
  {
  }

};


//----------------------------------------------------------------------------
ctkXnatResource::ctkXnatResource(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatResourcePrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatResource::~ctkXnatResource()
{
}

//----------------------------------------------------------------------------
QString ctkXnatResource::resourceUri() const
{
  return QString("%1/resources/%2").arg(parent()->resourceUri(), this->property("label"));
}

//----------------------------------------------------------------------------
void ctkXnatResource::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatResource::fetchImpl()
{
  QString resourceFilesUri = this->resourceUri() + "/files";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(resourceFilesUri);

  QList<ctkXnatObject*> files = session->httpResults(queryId,
                                                     ctkXnatDefaultSchemaTypes::XSI_FILE);

  foreach (ctkXnatObject* file, files)
  {
    QString label = file->property("name");
    if (label.isEmpty())
    {
      label = "NO NAME";
    }
    file->setProperty("label", label);
    this->add(file);
  }
}

//----------------------------------------------------------------------------
void ctkXnatResource::download(const QString& filename)
{
  this->session()->download(this, filename);
}
