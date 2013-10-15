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

#include "ctkXnatReconstruction.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"

class ctkXnatReconstructionPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatReconstructionPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }
  
  QString uri;
};


ctkXnatReconstruction::ctkXnatReconstruction()
: ctkXnatObject(*new ctkXnatReconstructionPrivate())
{
}

ctkXnatReconstruction::Pointer ctkXnatReconstruction::Create()
{
  Pointer ptr(new ctkXnatReconstruction());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatReconstruction::~ctkXnatReconstruction()
{
}

const QString& ctkXnatReconstruction::uri() const
{
  Q_D(const ctkXnatReconstruction);
  return d->uri;
}

void ctkXnatReconstruction::setUri(const QString& uri)
{
  Q_D(ctkXnatReconstruction);
  d->uri = uri;
}

void ctkXnatReconstruction::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatReconstruction::fetchImpl()
{
  Q_D(ctkXnatReconstruction);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->connection()->fetch(self.staticCast<ctkXnatReconstruction>());
}

void ctkXnatReconstruction::remove()
{
  // ctkXnatObject::remove();
  // getConnection()->remove(this);
}

bool ctkXnatReconstruction::isFile() const
{
  return false;
}
