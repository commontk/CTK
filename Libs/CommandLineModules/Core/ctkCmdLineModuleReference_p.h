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

#ifndef CTKCMDLINEMODULEREFERENCEPRIVATE_H
#define CTKCMDLINEMODULEREFERENCEPRIVATE_H

#include <ctkCmdLineModuleDescription.h>

#include <QSharedData>
#include <QUrl>

struct ctkCmdLineModuleBackend;
class ctkCmdLineModuleXmlException;

struct ctkCmdLineModuleReferencePrivate : public QSharedData
{
  ctkCmdLineModuleReferencePrivate();
  ~ctkCmdLineModuleReferencePrivate();

  ctkCmdLineModuleDescription description() const;

  ctkCmdLineModuleBackend* Backend;
  QUrl Location;
  QByteArray RawXmlDescription;
  QString XmlValidationErrorString;

private:

  mutable ctkCmdLineModuleDescription Description;
  mutable ctkCmdLineModuleXmlException* XmlException;
};

#endif // CTKCMDLINEMODULEREFERENCEPRIVATE_H
