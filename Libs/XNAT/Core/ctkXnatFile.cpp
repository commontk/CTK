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

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"


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
