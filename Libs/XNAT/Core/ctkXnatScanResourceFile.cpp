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

#include "ctkXnatScanResourceFile.h"

#include "ctkXnatConnection.h"

class ctkXnatScanResourceFilePrivate
{
public:
  QString name;
  QString size;
  QString uri;
  QString collection;
  QString fileTags;
  QString fileFormat;
  QString fileContent;
  QString categoryId;
};

ctkXnatScanResourceFile::ctkXnatScanResourceFile(ctkXnatObject* parent)
: ctkXnatObject(parent)
{
}

ctkXnatScanResourceFile::~ctkXnatScanResourceFile()
{
}

const QString& ctkXnatScanResourceFile::name() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->name;
}

void ctkXnatScanResourceFile::setName(const QString& name)
{
  Q_D(ctkXnatScanResourceFile);
  d->name = name;
}

const QString& ctkXnatScanResourceFile::size() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->size;
}

void ctkXnatScanResourceFile::setSize(const QString& size)
{
  Q_D(ctkXnatScanResourceFile);
  d->size = size;
}

const QString& ctkXnatScanResourceFile::uri() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->uri;
}

void ctkXnatScanResourceFile::setUri(const QString& uri)
{
  Q_D(ctkXnatScanResourceFile);
  d->uri = uri;
}

const QString& ctkXnatScanResourceFile::collection() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->collection;
}

void ctkXnatScanResourceFile::setCollection(const QString& collection)
{
  Q_D(ctkXnatScanResourceFile);
  d->collection = collection;
}

const QString& ctkXnatScanResourceFile::fileTags() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->fileTags;
}

void ctkXnatScanResourceFile::setFileTags(const QString& fileTags)
{
  Q_D(ctkXnatScanResourceFile);
  d->fileTags = fileTags;
}

const QString& ctkXnatScanResourceFile::fileFormat() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->fileFormat;
}

void ctkXnatScanResourceFile::setFileFormat(const QString& fileFormat)
{
  Q_D(ctkXnatScanResourceFile);
  d->fileFormat = fileFormat;
}

const QString& ctkXnatScanResourceFile::fileContent() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->fileContent;
}

void ctkXnatScanResourceFile::setFileContent(const QString& fileContent)
{
  Q_D(ctkXnatScanResourceFile);
  d->fileContent = fileContent;
}

const QString& ctkXnatScanResourceFile::categoryId() const
{
  Q_D(const ctkXnatScanResourceFile);
  return d->categoryId;
}

void ctkXnatScanResourceFile::setCategoryId(const QString& categoryId)
{
  Q_D(ctkXnatScanResourceFile);
  d->categoryId = categoryId;
}

void ctkXnatScanResourceFile::download(ctkXnatConnection* connection, const QString& fileName)
{
  connection->download(this, fileName);
}

bool ctkXnatScanResourceFile::isFile() const
{
  return true;
}

bool ctkXnatScanResourceFile::isDeletable() const
{
  return true;
}
