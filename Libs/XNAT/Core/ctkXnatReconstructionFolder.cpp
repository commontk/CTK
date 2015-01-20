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

#include "ctkXnatReconstructionFolder.h"

#include "ctkXnatSession.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatDefaultSchemaTypes.h"


//----------------------------------------------------------------------------
class ctkXnatReconstructionFolderPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionFolderPrivate()
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
ctkXnatReconstructionFolder::ctkXnatReconstructionFolder(ctkXnatObject* parent)
  : ctkXnatObject(*new ctkXnatReconstructionFolderPrivate(), parent, QString::null)
{
  this->setProperty("id", "reconstructions");
}

//----------------------------------------------------------------------------
ctkXnatReconstructionFolder::~ctkXnatReconstructionFolder()
{
}

//----------------------------------------------------------------------------
QString ctkXnatReconstructionFolder::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatReconstructionFolder::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatReconstructionFolder::fetchImpl()
{
  QString reconstructionsUri = this->resourceUri();
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(reconstructionsUri);

  QList<ctkXnatObject*> reconstructions = session->httpResults(queryId,
                                                               ctkXnatDefaultSchemaTypes::XSI_RECONSTRUCTION);

  foreach (ctkXnatObject* reconstruction, reconstructions)
  {
    this->add(reconstruction);
  }

}
