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

#include "ctkXnatExperiment.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSubject.h"
#include "ctkXnatScan.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatDefaultSchemaTypes.h"

//----------------------------------------------------------------------------
class ctkXnatExperimentPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatExperimentPrivate()
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
ctkXnatExperiment::ctkXnatExperiment(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatExperimentPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatExperiment::~ctkXnatExperiment()
{
}

//----------------------------------------------------------------------------
QString ctkXnatExperiment::resourceUri() const
{
  return QString("%1/experiments/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::fetchImpl()
{
  QString scansUri = this->resourceUri() + "/scans";
  ctkXnatSession* const session = this->session();
  QUuid scansQueryId = session->httpGet(scansUri);

  QList<ctkXnatObject*> scans = session->httpResults(scansQueryId,
                                                     ctkXnatDefaultSchemaTypes::XSI_SCAN);

  if (!scans.isEmpty())
  {
    ctkXnatScanFolder* scanFolder = new ctkXnatScanFolder();
    this->add(scanFolder);
  }

  QString reconstructionsUri = this->resourceUri() + "/reconstructions";
  QUuid reconstructionsQueryId = session->httpGet(reconstructionsUri);

  QList<ctkXnatObject*> reconstructions = session->httpResults(reconstructionsQueryId,
                                                               ctkXnatDefaultSchemaTypes::XSI_RECONSTRUCTION);

  if (!reconstructions.isEmpty())
  {
    ctkXnatReconstructionFolder* reconstructionFolder = new ctkXnatReconstructionFolder();
    this->add(reconstructionFolder);
  }
  this->fetchResources();
}
