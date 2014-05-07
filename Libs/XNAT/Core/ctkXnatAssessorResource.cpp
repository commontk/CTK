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

#include "ctkXnatAssessorResource.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <qDebug>

//----------------------------------------------------------------------------
class ctkXnatAssessorResourcePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatAssessorResourcePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
  }

};


//----------------------------------------------------------------------------
ctkXnatAssessorResource::ctkXnatAssessorResource(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatAssessorResourcePrivate(), parent, schemaType)
{
  qDebug() << " constructing  the assessor resource";
}

//----------------------------------------------------------------------------
ctkXnatAssessorResource::~ctkXnatAssessorResource()
{
}

//----------------------------------------------------------------------------
QString ctkXnatAssessorResource::resourceUri() const
{
  
  return QString("%1/resources/%2").arg(parent()->resourceUri(), this->property("xnat_abstractresource_id"));
}

//----------------------------------------------------------------------------
void ctkXnatAssessorResource::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatAssessorResource::fetchImpl()
{
  QString assessorResourceFilesUri = this->resourceUri() + "/files";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(assessorResourceFilesUri);

  
  QList<ctkXnatObject*> files = session->httpResults(queryId,
                                                     ctkXnatDefaultSchemaTypes::XSI_FILE);

  qDebug() << " trying to get things from : " << this->resourceUri() << "/files";
  foreach (ctkXnatObject* file, files)
  {
    QString label = file->property("Name");
    qDebug() << " got a assessment file called : " << label << ".";
    qDebug() << " with properties : " << file->properties() << ".";
    
    if (!label.isEmpty())
    {
      file->setProperty("ID", label);
    }
    this->add(file);
  }
}

//----------------------------------------------------------------------------
void ctkXnatAssessorResource::download(const QString& filename)
{
  this->session()->download(this, filename);
}
