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
#include "ctkXnatExperiment.h"
#include "ctkXnatScan.h"

ctkXnatScanFolder::ctkXnatScanFolder(ctkXnatObject* parent)
: ctkXnatObject(parent)
{
}

ctkXnatScanFolder::~ctkXnatScanFolder()
{
}

void ctkXnatScanFolder::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatScanFolder::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->downloadScanFiles(dynamic_cast<ctkXnatExperiment*>(getParent()), zipFileName);
}

QString ctkXnatScanFolder::getKind() const
{
  return "scan";
}

bool ctkXnatScanFolder::holdsFiles() const
{
  return true;
}
