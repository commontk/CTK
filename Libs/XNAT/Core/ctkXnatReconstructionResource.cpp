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
#include "ctkXnatObjectPrivate.h"

class ctkXnatReconstructionResourcePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionResourcePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }
  
//  QString uri;
};


ctkXnatReconstructionResource::ctkXnatReconstructionResource()
: ctkXnatObject(*new ctkXnatReconstructionResourcePrivate())
{
}

ctkXnatReconstructionResource::Pointer ctkXnatReconstructionResource::Create()
{
  Pointer ptr(new ctkXnatReconstructionResource());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatReconstructionResource::~ctkXnatReconstructionResource()
{
}

//const QString& ctkXnatReconstructionResource::uri() const
//{
//  Q_D(const ctkXnatReconstructionResource);
//  return d->uri;
//}

//void ctkXnatReconstructionResource::setUri(const QString& uri)
//{
//  Q_D(ctkXnatReconstructionResource);
//  d->uri = uri;
//}

void ctkXnatReconstructionResource::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatReconstructionResource::fetchImpl()
{
  Q_D(ctkXnatReconstructionResource);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->connection()->fetch(self.staticCast<ctkXnatReconstructionResource>());
}

void ctkXnatReconstructionResource::download(const QString& filename)
{
  this->connection()->download(this, filename);
}

void ctkXnatReconstructionResource::remove()
{
  // ctkXnatObject::remove();
  // getConnection()->remove(this);
}

bool ctkXnatReconstructionResource::isFile() const
{
  return false;
}
