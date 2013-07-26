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
#include "ctkXnatObjectPrivate.h"


class ctkXnatScanResourceFilePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanResourceFilePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }
  
  QString uri;
};


ctkXnatScanResourceFile::ctkXnatScanResourceFile()
: ctkXnatObject(new ctkXnatScanResourceFilePrivate())
{
}

ctkXnatScanResourceFile::Pointer ctkXnatScanResourceFile::Create()
{
  Pointer ptr(new ctkXnatScanResourceFile());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatScanResourceFile::~ctkXnatScanResourceFile()
{
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

void ctkXnatScanResourceFile::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatScanResourceFile::fetchImpl()
{

}

void ctkXnatScanResourceFile::remove()
{
  //connection->remove(this);
}

// void ctkXnatScanResourceFile::download(ctkXnatConnection* connection, const QString& zipFileName)
// {
//   connection->download(this, zipFileName);
// }
