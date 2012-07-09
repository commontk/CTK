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

#include <QStringList>
#include <QString>

#include <ctkCommandLineModulesExport.h>

#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleProcessFuture.h"

class QObject;

struct ctkCmdLineModuleDescriptionFactory
{
  virtual ~ctkCmdLineModuleDescriptionFactory() {}

  virtual QObject* createGUIFromXML(const QByteArray& xmlDescription)
  {
    Q_UNUSED(xmlDescription)
    return 0;
  }

  virtual QObject* createObjectRepresentationFromXML(const QByteArray& xmlDescription) = 0;
};

/// The methods in this class are for playing around... no API design yet
class CTK_CMDLINEMODULE_EXPORT ctkCmdLineModuleManager
{
public:
  ctkCmdLineModuleManager(ctkCmdLineModuleDescriptionFactory* descriptionFactory);

  ctkCmdLineModuleReference addModule(const QString& location);

  static QStringList createCommandLineArgs(QObject* hierarchy);

  static ctkCmdLineModuleProcessFuture run(const ctkCmdLineModuleReference& moduleRef);

private:

  ctkCmdLineModuleDescriptionFactory* descriptionFactory;

  QHash<QString, ctkCmdLineModuleReference> cache;
};

#endif // CTKCMDLINEMODULEMANAGER_H
