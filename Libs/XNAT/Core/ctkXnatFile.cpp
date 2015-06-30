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

#include "ctkXnatFile.h"

#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSession.h"

const QString ctkXnatFile::FILE_NAME = "Name";
const QString ctkXnatFile::FILE_TAGS = "file_tags";
const QString ctkXnatFile::FILE_FORMAT = "file_format";
const QString ctkXnatFile::FILE_CONTENT = "file_content";

//----------------------------------------------------------------------------
class ctkXnatFilePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatFilePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
  }

  QString localFilePath;
};


//----------------------------------------------------------------------------
ctkXnatFile::ctkXnatFile(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatFilePrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatFile::~ctkXnatFile()
{
}

//----------------------------------------------------------------------------
void ctkXnatFile::setName(const QString &name)
{
  this->setProperty(FILE_NAME, name);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::name() const
{
  return this->property(FILE_NAME);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileFormat(const QString &fileFormat)
{
  this->setProperty(FILE_FORMAT, fileFormat);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::fileFormat() const
{
  return this->property(FILE_FORMAT);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileContent(const QString &fileContent)
{
  this->setProperty(FILE_CONTENT, fileContent);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::fileContent() const
{
  return this->property(FILE_CONTENT);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileTags(const QString &fileTags)
{
  this->setProperty(FILE_TAGS, fileTags);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::fileTags() const
{
  return this->property(FILE_TAGS);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setLocalFilePath(const QString &filePath)
{
  Q_D(ctkXnatFile);
  d->localFilePath = filePath;
}

//----------------------------------------------------------------------------
QString ctkXnatFile::localFilePath() const
{
  Q_D(const ctkXnatFile);
  return d->localFilePath;
}

//----------------------------------------------------------------------------
QString ctkXnatFile::resourceUri() const
{
  return QString("%1/files/%2").arg(parent()->resourceUri(), this->name());
}

//----------------------------------------------------------------------------
void ctkXnatFile::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatFile::fetchImpl()
{
  // Does not make sense to fetch a file
}

//----------------------------------------------------------------------------
void ctkXnatFile::downloadImpl(const QString& filename)
{
  QString query = this->resourceUri();
  this->session()->download(filename, query);
}

//----------------------------------------------------------------------------
void ctkXnatFile::saveImpl(bool overwrite)
{
  Q_D(ctkXnatFile);

  ctkXnatSession::UrlParameters urlParams;
  urlParams["xsi:type"] = this->schemaType();
  // Flag needed for file upload
  urlParams["inbody"] = "true";

  // Creating the update query
  const QMap<QString, QString>& properties = this->properties();
  QMapIterator<QString, QString> itProperties(properties);
  while (itProperties.hasNext())
  {
    itProperties.next();

    // Do not append these file specific properties since they require a slightly
    // different key for uploading a file (e.g. instead of "file_format" only "format")
    if (itProperties.key() == FILE_TAGS || itProperties.key() == FILE_FORMAT ||
        itProperties.key() == FILE_CONTENT)
      continue;

    urlParams[itProperties.key()] = itProperties.value();
  }

  if (!this->fileFormat().isNull())
    urlParams["format"] = this->fileFormat();
  if (!this->fileContent().isNull())
    urlParams["content"] = this->fileContent();
  if (!this->fileContent().isNull())
    urlParams["tags"] = this->fileTags();

  if (this->exists() && overwrite)
    urlParams["overwrite"] = "true";

  this->session()->upload(this, urlParams);
}
