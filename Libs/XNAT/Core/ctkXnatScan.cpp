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

#include "ctkXnatScan.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"
// #include "ctkXnatScanResource.h"

class ctkXnatScanPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }
  
  QString uri;
};


ctkXnatScan::ctkXnatScan()
: ctkXnatObject(new ctkXnatScanPrivate())
{
}

ctkXnatScan::Pointer ctkXnatScan::Create()
{
  Pointer experiment(new ctkXnatScan());
  experiment->d_func()->selfPtr = experiment;
  return experiment;
}

ctkXnatScan::~ctkXnatScan()
{
}

const QString& ctkXnatScan::uri() const
{
  Q_D(const ctkXnatScan);
  return d->uri;
}

void ctkXnatScan::setUri(const QString& uri)
{
  Q_D(ctkXnatScan);
  d->uri = uri;
}

void ctkXnatScan::reset()
{
  Q_D(ctkXnatScan);
  ctkXnatObject::reset();
}

void ctkXnatScan::fetchImpl()
{
  Q_D(ctkXnatScan);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->getConnection()->fetch(self.staticCast<ctkXnatScan>());
}

void ctkXnatScan::remove()
{
  //connection->remove(this);
}

// void ctkXnatScan::download(ctkXnatConnection* connection, const QString& zipFileName)
// {
//   connection->download(this, zipFileName);
// }
