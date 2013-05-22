/*=============================================================================

  Plugin: org.commontk.xnat

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

#include "ctkXnatConnection.h"
#include "ctkXnatScanResource.h"

class ctkXnatScanPrivate
{
public:
  QString imageScanId;
  QString id;
  QString type;
  QString quality;
  QString xsiType;
  QString note;
  QString seriesDescription;
  QString uri;
};

ctkXnatScan::ctkXnatScan(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatScanPrivate())
{
}

ctkXnatScan::~ctkXnatScan()
{
}

const QString& ctkXnatScan::imageScanId() const
{
  Q_D(const ctkXnatScan);
  return d->imageScanId;
}

void ctkXnatScan::setImageScanId(const QString& imageScanId)
{
  Q_D(ctkXnatScan);
  d->imageScanId = imageScanId;
}

const QString& ctkXnatScan::id() const
{
  Q_D(const ctkXnatScan);
  return d->id;
}

void ctkXnatScan::setId(const QString& id)
{
  Q_D(ctkXnatScan);
  d->id = id;
}

const QString& ctkXnatScan::type() const
{
  Q_D(const ctkXnatScan);
  return d->type;
}

void ctkXnatScan::setType(const QString& type)
{
  Q_D(ctkXnatScan);
  d->type = type;
}

const QString& ctkXnatScan::quality() const
{
  Q_D(const ctkXnatScan);
  return d->quality;
}

void ctkXnatScan::setQuality(const QString& quality)
{
  Q_D(ctkXnatScan);
  d->quality = quality;
}

const QString& ctkXnatScan::xsiType() const
{
  Q_D(const ctkXnatScan);
  return d->xsiType;
}

void ctkXnatScan::setXsiType(const QString& xsiType)
{
  Q_D(ctkXnatScan);
  d->xsiType = xsiType;
}

const QString& ctkXnatScan::note() const
{
  Q_D(const ctkXnatScan);
  return d->note;
}

void ctkXnatScan::setNote(const QString& note)
{
  Q_D(ctkXnatScan);
  d->note = note;
}

const QString& ctkXnatScan::seriesDescription() const
{
  Q_D(const ctkXnatScan);
  return d->seriesDescription;
}

void ctkXnatScan::setSeriesDescription(const QString& seriesDescription)
{
  Q_D(ctkXnatScan);
  d->seriesDescription = seriesDescription;
}

const QString& ctkXnatScan::uri() const
{
  Q_D(const ctkXnatScan);
  return d->uri;
}

void ctkXnatScan::setUri(const QString& uri)
{
  Q_D(ctkXnatScan);
  d->uri = uri;
}

void ctkXnatScan::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatScan::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->download(this, zipFileName);
}

QString ctkXnatScan::getKind() const
{
  return "resource";
}

bool ctkXnatScan::holdsFiles() const
{
  return true;
}
