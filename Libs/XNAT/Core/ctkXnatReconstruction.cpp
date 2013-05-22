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

#include "ctkXnatReconstruction.h"

#include "ctkXnatConnection.h"
#include "ctkXnatReconstructionResource.h"

class ctkXnatReconstructionPrivate
{
public:
  QString reconstructedImageId;
  QString id;
  QString type;
  QString quality;
  QString baseScanType;
  QString note;
  QString seriesDescription;
  QString uri;
};

ctkXnatReconstruction::ctkXnatReconstruction(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatReconstructionPrivate())
{
}

ctkXnatReconstruction::~ctkXnatReconstruction()
{
}

const QString& ctkXnatReconstruction::reconstructedImageId() const
{
  Q_D(const ctkXnatReconstruction);
  return d->reconstructedImageId;
}

void ctkXnatReconstruction::setReconstructedImageId(const QString& reconstructeImageId)
{
  Q_D(ctkXnatReconstruction);
  d->reconstructedImageId = reconstructeImageId;
}

const QString& ctkXnatReconstruction::id() const
{
  Q_D(const ctkXnatReconstruction);
  return d->id;
}

void ctkXnatReconstruction::setId(const QString& id)
{
  Q_D(ctkXnatReconstruction);
  d->id = id;
}

const QString& ctkXnatReconstruction::type() const
{
  Q_D(const ctkXnatReconstruction);
  return d->type;
}

void ctkXnatReconstruction::setType(const QString& type)
{
  Q_D(ctkXnatReconstruction);
  d->type = type;
}

const QString& ctkXnatReconstruction::baseScanType() const
{
  Q_D(const ctkXnatReconstruction);
  return d->baseScanType;
}

void ctkXnatReconstruction::setBaseScanType(const QString& baseScanType)
{
  Q_D(ctkXnatReconstruction);
  d->baseScanType = baseScanType;
}

const QString& ctkXnatReconstruction::uri() const
{
  Q_D(const ctkXnatReconstruction);
  return d->uri;
}

void ctkXnatReconstruction::setUri(const QString& uri)
{
  Q_D(ctkXnatReconstruction);
  d->uri = uri;
}

void ctkXnatReconstruction::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatReconstruction::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->downloadReconstruction(this, zipFileName);
}

void ctkXnatReconstruction::add(ctkXnatConnection* connection, const QString& resource)
{
  connection->addReconstructionResource(this, resource);
}

void ctkXnatReconstruction::remove(ctkXnatConnection* connection)
{
  connection->removeReconstruction(this);
}

QString ctkXnatReconstruction::getKind() const
{
  return "resource";
}

bool ctkXnatReconstruction::holdsFiles() const
{
  return true;
}

bool ctkXnatReconstruction::receivesFiles() const
{
  return true;
}

bool ctkXnatReconstruction::isDeletable() const
{
  return true;
}
