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

#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleReferencePrivate.h"

ctkCmdLineModuleReference::ctkCmdLineModuleReference()
  : d(new ctkCmdLineModuleReferencePrivate)
{}

ctkCmdLineModuleReference::~ctkCmdLineModuleReference()
{
  if (!d->ref.deref()) delete d;
}

ctkCmdLineModuleReference::ctkCmdLineModuleReference(const ctkCmdLineModuleReference &ref)
  : d(ref.d)
{
  d->ref.ref();
}

ctkCmdLineModuleReference &ctkCmdLineModuleReference::operator =(const ctkCmdLineModuleReference &ref)
{
  ctkCmdLineModuleReferencePrivate* curr = d;
  d = ref.d;
  d->ref.ref();

  if (!curr->ref.deref()) delete curr;

  return *this;
}

ctkCmdLineModuleReference::operator bool()
{
  return isValid();
}

bool ctkCmdLineModuleReference::isValid()
{
  return !(d->xml.isEmpty() || d->objectRepresentation == 0);
}

QByteArray ctkCmdLineModuleReference::xmlDescription() const
{
  return d->xml;
}

QString ctkCmdLineModuleReference::location() const
{
  return d->loc;
}

QObject* ctkCmdLineModuleReference::widgetTree() const
{
  return d->gui;
}
