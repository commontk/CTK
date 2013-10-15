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

#include "ctkXnatReconstructionFolder.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"


class ctkXnatReconstructionFolderPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionFolderPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }
  
//  QString uri;
};


ctkXnatReconstructionFolder::ctkXnatReconstructionFolder()
: ctkXnatObject(*new ctkXnatReconstructionFolderPrivate())
{
  this->setProperty("ID", "Reconstructions");
}

ctkXnatReconstructionFolder::Pointer ctkXnatReconstructionFolder::Create()
{
  Pointer ptr(new ctkXnatReconstructionFolder());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatReconstructionFolder::~ctkXnatReconstructionFolder()
{
}

//const QString& ctkXnatReconstructionFolder::uri() const
//{
//  Q_D(const ctkXnatReconstructionFolder);
//  return d->uri;
//}

//void ctkXnatReconstructionFolder::setUri(const QString& uri)
//{
//  Q_D(ctkXnatReconstructionFolder);
//  d->uri = uri;
//}

void ctkXnatReconstructionFolder::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatReconstructionFolder::fetchImpl()
{
  Q_D(ctkXnatReconstructionFolder);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->connection()->fetch(self.staticCast<ctkXnatReconstructionFolder>());
}

void ctkXnatReconstructionFolder::remove()
{
  // ctkXnatObject::remove();
  // getConnection()->remove(this);
}

bool ctkXnatReconstructionFolder::isFile() const
{
  return false;
}
