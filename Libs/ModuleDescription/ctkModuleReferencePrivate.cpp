/*=============================================================================
  
  Library: CTK
  
  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics
    
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

#include "ctkModuleReferencePrivate.h"

ctkModuleReferencePrivate::ctkModuleReferencePrivate()
  : objectRepresentation(0), ref(1), gui(0)
{}

ctkModuleReferencePrivate::~ctkModuleReferencePrivate()
{
  objectRepresentation->deleteLater();
  if (gui) gui->deleteLater();
}

void ctkModuleReferencePrivate::setGUI(QObject* gui)
{
  if (this->gui) disconnect(gui);
  this->gui = gui;
  connect(this->gui, SIGNAL(destroyed()), this, SLOT(guiDestroyed()));
}

void ctkModuleReferencePrivate::guiDestroyed()
{
  gui = 0;
}
