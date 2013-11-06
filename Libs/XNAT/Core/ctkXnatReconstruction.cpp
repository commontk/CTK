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


ctkXnatReconstruction::ctkXnatReconstruction(const QString& schemaType)
: ctkXnatObject(*new ctkXnatReconstructionPrivate(), schemaType)
{
}

ctkXnatReconstruction::~ctkXnatReconstruction()
{
}

QString ctkXnatReconstruction::resourceUri() const
{
  return QString("%1/reconstructions/%2").arg(parent()->resourceUri(), this->id());
}

void ctkXnatReconstruction::reset()
{
  ctkXnatObject::reset();
}

void ctkXnatReconstruction::fetchImpl()
{
  this->connection()->fetch(this);
}
