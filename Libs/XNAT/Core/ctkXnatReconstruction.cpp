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

#include "ctkXnatReconstruction.h"

#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatFile.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatSession.h"

#include <QDebug>

//----------------------------------------------------------------------------
class ctkXnatReconstructionPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }

  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatReconstruction::ctkXnatReconstruction(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatReconstructionPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatReconstruction::~ctkXnatReconstruction()
{
}

//----------------------------------------------------------------------------
QString ctkXnatReconstruction::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatReconstruction::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatReconstruction::fetchImpl()
{
  QString reconstructionResourcesUri = this->resourceUri() + "/files";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(reconstructionResourcesUri);

  QList<ctkXnatObject*> reconstructionResources = session->httpResults(queryId,
                                                                       ctkXnatDefaultSchemaTypes::XSI_FILE);

  foreach (ctkXnatObject* reconstructionResource, reconstructionResources)
  {
    QString label = reconstructionResource->name();
    if (!label.isEmpty())
    {
      reconstructionResource->setName(label);
    }

    this->add(reconstructionResource);
  }
}

//----------------------------------------------------------------------------
void ctkXnatReconstruction::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatReconstruction::downloadImpl(const QString& filename) not yet tested";
  QString query = this->resourceUri() + "/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}
