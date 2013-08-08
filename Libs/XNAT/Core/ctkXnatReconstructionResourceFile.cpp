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
#include "ctkXnatObjectPrivate.h"


class ctkXnatReconstructionResourceFilePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionResourceFilePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }
  
//  QString uri;
};

ctkXnatReconstructionResourceFile::ctkXnatReconstructionResourceFile()
: ctkXnatObject(new ctkXnatReconstructionResourceFilePrivate())
{
}

ctkXnatReconstructionResourceFile::Pointer ctkXnatReconstructionResourceFile::Create()
{
  Pointer ptr(new ctkXnatReconstructionResourceFile());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatReconstructionResourceFile::~ctkXnatReconstructionResourceFile()
{
}

//const QString& ctkXnatReconstructionResourceFile::uri() const
//{
//  Q_D(const ctkXnatReconstructionResourceFile);
//  return d->uri;
//}

//void ctkXnatReconstructionResourceFile::setUri(const QString& uri)
//{
//  Q_D(ctkXnatReconstructionResourceFile);
//  d->uri = uri;
//}

void ctkXnatReconstructionResourceFile::download(const QString& fileName)
{
  this->connection()->download(this, fileName);
}

void ctkXnatReconstructionResourceFile::upload(const QString& fileName)
{
}


void ctkXnatReconstructionResourceFile::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatReconstructionResourceFile::fetchImpl()
{

}

void ctkXnatReconstructionResourceFile::remove()
{
  // ctkXnatObject::remove();
  // getConnection()->remove(this);
}

bool ctkXnatReconstructionResourceFile::isFile() const
{
  return true;
}
