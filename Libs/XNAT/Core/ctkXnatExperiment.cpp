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

#include "ctkXnatExperiment.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"

class ctkXnatExperimentPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatExperimentPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }
  
  QString uri;
};


ctkXnatExperiment::ctkXnatExperiment()
: ctkXnatObject(new ctkXnatExperimentPrivate())
{
}

ctkXnatExperiment::Pointer ctkXnatExperiment::Create()
{
  Pointer ptr(new ctkXnatExperiment());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatExperiment::~ctkXnatExperiment()
{
}

const QString& ctkXnatExperiment::uri() const
{
  Q_D(const ctkXnatExperiment);
  return d->uri;
}

void ctkXnatExperiment::setUri(const QString& uri)
{
  Q_D(ctkXnatExperiment);
  d->uri = uri;
}

void ctkXnatExperiment::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatExperiment::fetchImpl()
{
  Q_D(ctkXnatExperiment);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->getConnection()->fetch(self.staticCast<ctkXnatExperiment>());
}

void ctkXnatExperiment::remove()
{
  //connection->remove(this);
}
