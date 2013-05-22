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

#include "ctkXnatReconstructionResource.h"

#include "ctkXnatConnection.h"
#include "ctkXnatReconstructionResourceFile.h"

class ctkXnatReconstructionResourcePrivate
{
public:
  QString resourceId;
  QString label;
  QString elementName;
  QString category;
  QString categoryId;
  QString categoryDescription;
};

ctkXnatReconstructionResource::ctkXnatReconstructionResource(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatReconstructionResourcePrivate())
{
}

ctkXnatReconstructionResource::~ctkXnatReconstructionResource()
{
}

const QString& ctkXnatReconstructionResource::resourceId() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->resourceId;
}

void ctkXnatReconstructionResource::setResourceId(const QString& resourceId)
{
  Q_D(ctkXnatReconstructionResource);
  d->resourceId = resourceId;
}

const QString& ctkXnatReconstructionResource::label() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->label;
}

void ctkXnatReconstructionResource::setLabel(const QString& label)
{
  Q_D(ctkXnatReconstructionResource);
  d->label = label;
}

const QString& ctkXnatReconstructionResource::elementName() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->elementName;
}

void ctkXnatReconstructionResource::setElementName(const QString& elementName)
{
  Q_D(ctkXnatReconstructionResource);
  d->elementName = elementName;
}

const QString& ctkXnatReconstructionResource::category() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->category;
}

void ctkXnatReconstructionResource::setCategory(const QString& category)
{
  Q_D(ctkXnatReconstructionResource);
  d->category = category;
}

const QString& ctkXnatReconstructionResource::categoryId() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->categoryId;
}

void ctkXnatReconstructionResource::setCategoryId(const QString& categoryId)
{
  Q_D(ctkXnatReconstructionResource);
  d->categoryId = categoryId;
}

const QString& ctkXnatReconstructionResource::categoryDescription() const
{
  Q_D(const ctkXnatReconstructionResource);
  return d->categoryDescription;
}

void ctkXnatReconstructionResource::setCategoryDescription(const QString& categoryDescription)
{
  Q_D(ctkXnatReconstructionResource);
  d->categoryDescription = categoryDescription;
}

void ctkXnatReconstructionResource::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatReconstructionResource::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->downloadReconstructionResourceFiles(this, zipFileName);
}

void ctkXnatReconstructionResource::upload(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->uploadReconstructionResourceFiles(this, zipFileName);
}

void ctkXnatReconstructionResource::remove(ctkXnatConnection* connection)
{
  connection->removeReconstructionResource(this);
}

bool ctkXnatReconstructionResource::isFile() const
{
  return true;
}

bool ctkXnatReconstructionResource::isDeletable() const
{
  return true;
}
