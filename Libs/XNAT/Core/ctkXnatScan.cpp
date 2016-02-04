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

#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatSession.h"

const QString ctkXnatScan::QUALITY = "quality";
const QString ctkXnatScan::SERIES_DESCRIPTION = "series_description";
const QString ctkXnatScan::TYPE = "type";

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
void ctkXnatScan::setQuality(const QString &quality)
{
  this->setProperty(QUALITY, quality);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::quality() const
{
  return this->property(QUALITY);
}

//----------------------------------------------------------------------------
void ctkXnatScan::setSeriesDescription(const QString &seriesDescription)
{
  this->setProperty(SERIES_DESCRIPTION, seriesDescription);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::seriesDescription() const
{
  return this->property(SERIES_DESCRIPTION);
}

//----------------------------------------------------------------------------
void ctkXnatScan::setType(const QString &type)
{
  this->setProperty(TYPE, type);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::type() const
{
  return this->property(TYPE);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::resourceUri() const
{
  return QString("%1/%2/resources").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatScan::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatScan::fetchImpl()
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
void ctkXnatScan::downloadImpl(const QString& filename)
{
  QString query = this->resourceUri() + "/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}
