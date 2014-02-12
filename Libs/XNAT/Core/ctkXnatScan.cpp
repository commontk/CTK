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

#include "ctkXnatScan.h"

#include "ctkXnatSession.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatScanResource.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"


//----------------------------------------------------------------------------
class ctkXnatScanPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanPrivate()
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
ctkXnatScan::ctkXnatScan(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatScanPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatScan::~ctkXnatScan()
{
}

//----------------------------------------------------------------------------
QString ctkXnatScan::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatScan::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatScan::fetchImpl()
{
  QString scanResourcesUri = this->resourceUri() + "/resources";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(scanResourcesUri);

  QList<ctkXnatObject*> scanResources = session->httpResults(queryId,
                                                             ctkXnatDefaultSchemaTypes::XSI_SCAN_RESOURCE);

  foreach (ctkXnatObject* scanResource, scanResources)
  {
    QString label = scanResource->property("label");
    if (!label.isEmpty())
    {
      scanResource->setProperty("ID", label);
    }
    this->add(scanResource);
  }
}
