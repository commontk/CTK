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
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <QDebug>

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
  this->fetchResources();
}

//----------------------------------------------------------------------------
void ctkXnatScan::downloadImpl(const QString& filename)
{
  QString query = this->resourceUri() + "/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}
