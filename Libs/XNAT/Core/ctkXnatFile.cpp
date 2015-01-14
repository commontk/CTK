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

#include "ctkXnatConstants.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSession.h"


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
//    uri.clear();
  }

//  QString uri;
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
  setProperty(ctkXnatObjectFields::FILE_NAME, name);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::name() const
{
  return property(ctkXnatObjectFields::FILE_NAME);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileFormat(const QString &fileFormat)
{
  setProperty(ctkXnatObjectFields::FILE_FORMAT, fileFormat);
}

QString ctkXnatFile::fileFormat() const
{
  return property(ctkXnatObjectFields::FILE_FORMAT);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileContent(const QString &fileContent)
{
  setProperty(ctkXnatObjectFields::FILE_CONTENT, fileContent);
}

QString ctkXnatFile::fileContent() const
{
  return property(ctkXnatObjectFields::FILE_CONTENT);
}

//----------------------------------------------------------------------------
void ctkXnatFile::setFileTags(const QString &fileTags)
{
  setProperty(ctkXnatObjectFields::FILE_TAGS, fileTags);
}

QString ctkXnatFile::fileTags() const
{
  return property(ctkXnatObjectFields::FILE_TAGS);
}

//----------------------------------------------------------------------------
QString ctkXnatFile::resourceUri() const
{
  return QString("%1/files/%2").arg(parent()->resourceUri(), this->name());
}

//----------------------------------------------------------------------------
void ctkXnatFile::download(const QString& filename)
{
  this->session()->download(this, filename);
}

//----------------------------------------------------------------------------
void ctkXnatFile::upload(const QString& /*filename*/)
{
}

//----------------------------------------------------------------------------
void ctkXnatFile::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatFile::fetchImpl()
{
}
