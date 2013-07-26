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

#include "ctkXnatScanFolder.h"

#include "ctkXnatConnection.h"
#include "ctkXnatObjectPrivate.h"


class ctkXnatScanFolderPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatScanFolderPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }
  
  QString uri;
};


ctkXnatScanFolder::ctkXnatScanFolder()
: ctkXnatObject(new ctkXnatScanFolderPrivate())
{
  this->setProperty("ID", "scans");
}

ctkXnatScanFolder::Pointer ctkXnatScanFolder::Create()
{
  Pointer ptr(new ctkXnatScanFolder());
  ptr->d_func()->selfPtr = ptr;
  return ptr;
}

ctkXnatScanFolder::~ctkXnatScanFolder()
{
}

const QString& ctkXnatScanFolder::uri() const
{
  Q_D(const ctkXnatScanFolder);
  return d->uri;
}

void ctkXnatScanFolder::setUri(const QString& uri)
{
  Q_D(ctkXnatScanFolder);
  d->uri = uri;
}

void ctkXnatScanFolder::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatScanFolder::fetchImpl()
{
  Q_D(ctkXnatScanFolder);
  ctkXnatObject::Pointer self = d->selfPtr;
  this->getConnection()->fetch(self.staticCast<ctkXnatScanFolder>());
}

void ctkXnatScanFolder::remove()
{
  //connection->remove(this);
}

// void ctkXnatScanFolder::download(ctkXnatConnection* connection, const QString& zipFileName)
// {
//   connection->downloadScanFiles(dynamic_cast<ctkXnatExperiment*>(getParent()), zipFileName);
// }
