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

#ifndef CTKCMDLINEMODULEMANAGER_H
#define CTKCMDLINEMODULEMANAGER_H

#include <ctkCommandLineModulesCoreExport.h>

#include <QStringList>
#include <QString>

struct ctkCmdLineModuleInstanceFactory;

class ctkCmdLineModuleInstance;
class ctkCmdLineModuleReference;
class ctkCmdLineModuleManagerPrivate;

/// The methods in this class are for playing around... no API design yet
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleManager
{

public:

  enum ValidationMode {
    /** registerModule() will throw an exception if the XML is invalid */
    STRICT_VALIDATION,
    /** no XML schema validation at all */
    SKIP_VALIDATION,
    /**
     * registerModule() will validate the XML description but proceed with
     * registration on validation error.
     */
    WEAK_VALIDATION
  };

  ctkCmdLineModuleManager(ctkCmdLineModuleInstanceFactory* descriptionFactory,
                          ValidationMode = STRICT_VALIDATION);

  ~ctkCmdLineModuleManager();

  ctkCmdLineModuleReference registerModule(const QString& location);
  void unregisterModule(const ctkCmdLineModuleReference& moduleRef);

  ctkCmdLineModuleReference moduleReference(const QString& location) const;
  QList<ctkCmdLineModuleReference> moduleReferences() const;

  ctkCmdLineModuleInstance* createModuleInstance(const ctkCmdLineModuleReference& moduleRef);

  QList<ctkCmdLineModuleInstance*> moduleInstances(const ctkCmdLineModuleReference& moduleRef) const;

  //static QStringList createCommandLineArgs(QObject* hierarchy);

  //static ctkCmdLineModuleProcessFuture run(const ctkCmdLineModuleReference& moduleRef);

private:

  QScopedPointer<ctkCmdLineModuleManagerPrivate> d;

  Q_DISABLE_COPY(ctkCmdLineModuleManager)

};

#endif // CTKCMDLINEMODULEMANAGER_H
