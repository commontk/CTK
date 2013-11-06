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

#include "ctkXnatScanResource.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"

class ctkXnatScanResourcePrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanResourcePrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }
  
//  QString uri;
};


ctkXnatScanResource::ctkXnatScanResource()
: ctkXnatObject(*new ctkXnatScanResourcePrivate())
{
}

ctkXnatScanResource::~ctkXnatScanResource()
{
}

//const QString& ctkXnatScanResource::uri() const
//{
//  Q_D(const ctkXnatScanResource);
//  return d->uri;
//}

//void ctkXnatScanResource::setUri(const QString& uri)
//{
//  Q_D(ctkXnatScanResource);
//  d->uri = uri;
//}

void ctkXnatScanResource::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatScanResource::fetchImpl()
{
  this->connection()->fetch(this);
}

void ctkXnatScanResource::download(const QString& filename)
{
  this->connection()->download(this, filename);
}
