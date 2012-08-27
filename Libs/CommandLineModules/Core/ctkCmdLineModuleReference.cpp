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
#include "ctkCmdLineModuleReference_p.h"
#include "ctkCmdLineModuleXmlParser_p.h"

#include <QBuffer>


//----------------------------------------------------------------------------
ctkCmdLineModuleReferencePrivate::ctkCmdLineModuleReferencePrivate()
  : Backend(NULL)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription ctkCmdLineModuleReferencePrivate::description() const
{
  // Lazy creation. The title is a required XML element.
  if (Description.title().isNull())
  {
    QByteArray xml(RawXmlDescription);
    QBuffer xmlInput(&xml);
    ctkCmdLineModuleXmlParser parser(&xmlInput, &Description);
    parser.doParse();
  }
  return Description;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleReference::ctkCmdLineModuleReference()
  : d(new ctkCmdLineModuleReferencePrivate())
{}

//----------------------------------------------------------------------------
ctkCmdLineModuleReference::~ctkCmdLineModuleReference()
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleReference::ctkCmdLineModuleReference(const ctkCmdLineModuleReference &ref)
  : d(ref.d)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleReference &ctkCmdLineModuleReference::operator =(const ctkCmdLineModuleReference &ref)
{
  d = ref.d;
  return *this;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleReference::operator bool() const
{
  return !d->RawXmlDescription.isEmpty();
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription ctkCmdLineModuleReference::description() const
{
  return d->description();
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleReference::rawXmlDescription() const
{
  return d->RawXmlDescription;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleReference::xmlValidationErrorString() const
{
  return d->XmlValidationErrorString;
}

//----------------------------------------------------------------------------
QUrl ctkCmdLineModuleReference::location() const
{
  return d->Location;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackend *ctkCmdLineModuleReference::backend() const
{
  return d->Backend;
}

//----------------------------------------------------------------------------
uint qHash(const ctkCmdLineModuleReference& moduleRef)
{
  return qHash(moduleRef.d.data());
}
