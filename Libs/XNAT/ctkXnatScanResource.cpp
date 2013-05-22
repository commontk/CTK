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

#include "ctkXnatScanResource.h"

#include "ctkXnatConnection.h"
#include "ctkXnatScanResourceFile.h"

#include <QDebug>

class ctkXnatScanResourcePrivate
{
public:
  QString resourceId;
  QString label;
  QString elementName;
  QString category;
  QString categoryId;
  QString categoryDescription;
};

ctkXnatScanResource::ctkXnatScanResource(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatScanResourcePrivate())
{
}

ctkXnatScanResource::~ctkXnatScanResource()
{
}

const QString& ctkXnatScanResource::resourceId() const
{
  Q_D(const ctkXnatScanResource);
  return d->resourceId;
}

void ctkXnatScanResource::setResourceId(const QString& resourceId)
{
  Q_D(ctkXnatScanResource);
  d->resourceId = resourceId;
}

const QString& ctkXnatScanResource::label() const
{
  Q_D(const ctkXnatScanResource);
  return d->label;
}

void ctkXnatScanResource::setLabel(const QString& label)
{
  Q_D(ctkXnatScanResource);
  d->label = label;
}

const QString& ctkXnatScanResource::elementName() const
{
  Q_D(const ctkXnatScanResource);
  return d->elementName;
}

void ctkXnatScanResource::setElementName(const QString& elementName)
{
  Q_D(ctkXnatScanResource);
  d->elementName = elementName;
}

const QString& ctkXnatScanResource::category() const
{
  Q_D(const ctkXnatScanResource);
  return d->category;
}

void ctkXnatScanResource::setCategory(const QString& category)
{
  Q_D(ctkXnatScanResource);
  d->category = category;
}

const QString& ctkXnatScanResource::categoryId() const
{
  Q_D(const ctkXnatScanResource);
  return d->categoryId;
}

void ctkXnatScanResource::setCategoryId(const QString& categoryId)
{
  Q_D(ctkXnatScanResource);
  d->categoryId = categoryId;
}

const QString& ctkXnatScanResource::categoryDescription() const
{
  Q_D(const ctkXnatScanResource);
  return d->categoryDescription;
}

void ctkXnatScanResource::setCategoryDescription(const QString& categoryDescription)
{
  Q_D(ctkXnatScanResource);
  d->categoryDescription = categoryDescription;
}

void ctkXnatScanResource::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatScanResource::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->download(this, zipFileName);
}

bool ctkXnatScanResource::isFile() const
{
  return true;
}
