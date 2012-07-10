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
  : d(new ctkCmdLineModuleReferencePrivate())
{}

ctkCmdLineModuleReference::~ctkCmdLineModuleReference()
{
}

ctkCmdLineModuleReference::ctkCmdLineModuleReference(const ctkCmdLineModuleReference &ref)
  : d(ref.d)
{
}

ctkCmdLineModuleReference &ctkCmdLineModuleReference::operator =(const ctkCmdLineModuleReference &ref)
{
  d = ref.d;
  return *this;
}

ctkCmdLineModuleReference::operator bool()
{
  return d;
}

ctkCmdLineModuleDescription ctkCmdLineModuleReference::description() const
{
  return d->Description;
}

QByteArray ctkCmdLineModuleReference::rawXmlDescription() const
{
  return d->RawXmlDescription;
}

QString ctkCmdLineModuleReference::location() const
{
  return d->Location;
}
