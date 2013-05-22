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

#include "ctkXnatReconstructionResourceFile.h"

#include "ctkXnatConnection.h"

class ctkXnatReconstructionResourceFilePrivate
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

ctkXnatReconstructionResourceFile::ctkXnatReconstructionResourceFile(ctkXnatObject* parent)
: ctkXnatObject(parent)
{
}

ctkXnatReconstructionResourceFile::~ctkXnatReconstructionResourceFile()
{
}

const QString& ctkXnatReconstructionResourceFile::name() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->name;
}

void ctkXnatReconstructionResourceFile::setName(const QString& name)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->name = name;
}

const QString& ctkXnatReconstructionResourceFile::size() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->size;
}

void ctkXnatReconstructionResourceFile::setSize(const QString& size)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->size = size;
}

const QString& ctkXnatReconstructionResourceFile::uri() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->uri;
}

void ctkXnatReconstructionResourceFile::setUri(const QString& uri)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->uri = uri;
}

const QString& ctkXnatReconstructionResourceFile::collection() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->collection;
}

void ctkXnatReconstructionResourceFile::setCollection(const QString& collection)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->collection = collection;
}

const QString& ctkXnatReconstructionResourceFile::fileTags() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->fileTags;
}

void ctkXnatReconstructionResourceFile::setFileTags(const QString& fileTags)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->fileTags = fileTags;
}

const QString& ctkXnatReconstructionResourceFile::fileFormat() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->fileFormat;
}

void ctkXnatReconstructionResourceFile::setFileFormat(const QString& fileFormat)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->fileFormat = fileFormat;
}

const QString& ctkXnatReconstructionResourceFile::fileContent() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->fileContent;
}

void ctkXnatReconstructionResourceFile::setFileContent(const QString& fileContent)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->fileContent = fileContent;
}

const QString& ctkXnatReconstructionResourceFile::categoryId() const
{
  Q_D(const ctkXnatReconstructionResourceFile);
  return d->categoryId;
}

void ctkXnatReconstructionResourceFile::setCategoryId(const QString& categoryId)
{
  Q_D(ctkXnatReconstructionResourceFile);
  d->categoryId = categoryId;
}

void ctkXnatReconstructionResourceFile::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->download(this, zipFileName);
}

void ctkXnatReconstructionResourceFile::remove(ctkXnatConnection* connection)
{
  connection->remove(this);
}

bool ctkXnatReconstructionResourceFile::isFile() const
{
  return true;
}

bool ctkXnatReconstructionResourceFile::isDeletable() const
{
  return true;
}
