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

#include "ctkCmdLineModuleExplorerGeneralModuleSettings.h"
#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleExplorerConstants.h"

#include <QThreadPool>
#include <QSettings>

ctkCmdLineModuleExplorerGeneralModuleSettings::ctkCmdLineModuleExplorerGeneralModuleSettings(ctkCmdLineModuleManager* cmdLineModuleManager)
  : CmdLineModuleManager(cmdLineModuleManager)
{
  this->setupUi(this);

  this->registerProperty(ctkCmdLineModuleExplorerConstants::KEY_MAX_PARALLEL_MODULES,
                         this->MaxParallelModules, "value", SIGNAL(valueChanged(int)));
  this->registerProperty(ctkCmdLineModuleExplorerConstants::KEY_XML_TIMEOUT_SECONDS,
                         this->XmlTimeout, "value", SIGNAL(valueChanged(int)));
}

void ctkCmdLineModuleExplorerGeneralModuleSettings::applySettings()
{
  int maxParallelModules = this->propertyValue(ctkCmdLineModuleExplorerConstants::KEY_MAX_PARALLEL_MODULES).toInt();
  QThreadPool::globalInstance()->setMaxThreadCount(maxParallelModules);

  int timeout = this->propertyValue(ctkCmdLineModuleExplorerConstants::KEY_XML_TIMEOUT_SECONDS).toInt();
  this->CmdLineModuleManager->setTimeOutForXMLRetrieval(timeout*1000);
}
