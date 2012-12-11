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

#include "ctkCmdLineModuleExplorerDirectorySettings.h"
#include "ctkCmdLineModuleExplorerConstants.h"

#include <ctkPathListWidget.h>
#include <ctkCmdLineModuleDirectoryWatcher.h>

#include <QVBoxLayout>

ctkCmdLineModuleExplorerDirectorySettings::ctkCmdLineModuleExplorerDirectorySettings(ctkCmdLineModuleDirectoryWatcher *directoryWatcher)
  : DirectoryWatcher(directoryWatcher)
{
  this->setupUi(this);

  this->PathListButtonsWidget->init(this->PathListWidget);

  this->registerProperty(ctkCmdLineModuleExplorerConstants::KEY_SEARCH_PATHS,
                         this->PathListWidget, "paths", SIGNAL(pathsChanged(QStringList,QStringList)));
}

void ctkCmdLineModuleExplorerDirectorySettings::applySettings()
{
  QVariant newSearchPaths = this->propertyValue(ctkCmdLineModuleExplorerConstants::KEY_SEARCH_PATHS);
  this->DirectoryWatcher->setDirectories(newSearchPaths.toStringList());

  ctkSettingsPanel::applySettings();
}
