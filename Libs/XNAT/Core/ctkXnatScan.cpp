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

#include "ctkXnatConstants.h"
#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatSession.h"

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
  this->setProperty(ctkXnatObjectFields::QUALITY, quality);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::quality() const
{
  return this->property(ctkXnatObjectFields::QUALITY);
}

//----------------------------------------------------------------------------
void ctkXnatScan::setSeriesDescription(const QString &seriesDescription)
{
  this->setProperty(ctkXnatObjectFields::SERIES_DESCRIPTION, seriesDescription);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::seriesDescription() const
{
  return this->property(ctkXnatObjectFields::SERIES_DESCRIPTION);
}

//----------------------------------------------------------------------------
void ctkXnatScan::setType(const QString &type)
{
  this->setProperty(ctkXnatObjectFields::TYPE, type);
}

//----------------------------------------------------------------------------
QString ctkXnatScan::type() const
{
  return this->property(ctkXnatObjectFields::TYPE);
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
