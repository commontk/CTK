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

#ifndef CTKCMDLINEMODULEREFERENCE_H
#define CTKCMDLINEMODULEREFERENCE_H

#include <ctkCommandLineModulesCoreExport.h>

#include <QSharedDataPointer>

class ctkCmdLineModuleDescription;
class ctkCmdLineModuleReferencePrivate;

class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleReference
{
public:

  ctkCmdLineModuleReference();
  ~ctkCmdLineModuleReference();

  ctkCmdLineModuleReference(const ctkCmdLineModuleReference& ref);
  ctkCmdLineModuleReference& operator=(const ctkCmdLineModuleReference& ref);

  operator bool();

  ctkCmdLineModuleDescription description() const;

  QByteArray rawXmlDescription() const;

  QString location() const;

private:

  friend class ctkCmdLineModuleManager;

  QSharedDataPointer<ctkCmdLineModuleReferencePrivate> d;

};

#endif // CTKCMDLINEMODULEREFERENCE_H
