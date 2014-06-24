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

#include "ctkXnatScanResource.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatScan.h"
#include "ctkXnatFile.h"
#include "ctkXnatDefaultSchemaTypes.h"


//----------------------------------------------------------------------------
class ctkXnatScanResourcePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanResourcePrivate()
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
ctkXnatScanResource::ctkXnatScanResource(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatScanResourcePrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatScanResource::~ctkXnatScanResource()
{
}

//----------------------------------------------------------------------------
QString ctkXnatScanResource::resourceUri() const
{
  return QString("%1/resources/%2").arg(parent()->resourceUri(), this->property("label"));
}

//----------------------------------------------------------------------------
void ctkXnatScanResource::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatScanResource::fetchImpl()
{
  QString scanResourceFilesUri = this->resourceUri() + "/files";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(scanResourceFilesUri);

  QList<ctkXnatObject*> files = session->httpResults(queryId,
                                                     ctkXnatDefaultSchemaTypes::XSI_FILE);

  foreach (ctkXnatObject* file, files)
  {
    QString label = file->property("Name");
    if (!label.isEmpty())
    {
      file->setProperty("ID", label);
    }
    this->add(file);
  }
}

//----------------------------------------------------------------------------
void ctkXnatScanResource::download(const QString& filename)
{
  this->session()->download(this, filename);
}
