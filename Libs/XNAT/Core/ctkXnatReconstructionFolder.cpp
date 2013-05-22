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
#include "ctkXnatExperiment.h"
#include "ctkXnatReconstruction.h"

ctkXnatReconstructionFolder::ctkXnatReconstructionFolder(ctkXnatObject* parent)
: ctkXnatObject(parent)
{
}

ctkXnatReconstructionFolder::~ctkXnatReconstructionFolder()
{
}

void ctkXnatReconstructionFolder::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

void ctkXnatReconstructionFolder::download(ctkXnatConnection* connection, const QString& zipFileName)
{
  connection->downloadReconstructionFiles(dynamic_cast<ctkXnatExperiment*>(getParent()), zipFileName);
}

void ctkXnatReconstructionFolder::add(ctkXnatConnection* connection, const QString& categoryEntry)
{
  connection->addReconstruction(dynamic_cast<ctkXnatExperiment*>(getParent()), categoryEntry);
}

QString ctkXnatReconstructionFolder::getModifiableChildKind() const
{
  return "reconstruction";
}

QString ctkXnatReconstructionFolder::getModifiableParentName() const
{
  return getParent()->getName();
}

bool ctkXnatReconstructionFolder::isModifiable() const
{
  return true;
}

QString ctkXnatReconstructionFolder::getKind() const
{
  return "reconstruction";
}

QString ctkXnatReconstructionFolder::getModifiableChildKind(int parentIndex) const
{
  return "resource";
}

QString ctkXnatReconstructionFolder::getModifiableParentName(int parentIndex) const
{
  return this->childName(parentIndex);
}

bool ctkXnatReconstructionFolder::holdsFiles() const
{
  return true;
}

bool ctkXnatReconstructionFolder::isModifiable(int parentIndex) const
{
  return true;
}

bool ctkXnatReconstructionFolder::isDeletable() const
{
  return true;
}
