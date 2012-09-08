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

#ifndef CTKCMDLINEMODULEEXPLORERDIRECTORYSETTINGS_H
#define CTKCMDLINEMODULEEXPLORERDIRECTORYSETTINGS_H

#include <ctkSettingsPanel.h>

#include "ui_ctkCmdLineModuleExplorerDirectorySettings.h"

class ctkCmdLineModuleDirectoryWatcher;

/**
 * \class ctkCmdLineModuleExplorerDirectorySettings
 * \brief Example application settings panel.
 */
class ctkCmdLineModuleExplorerDirectorySettings : public ctkSettingsPanel, public Ui::ctkCmdLineModuleExplorerDirectorySettings
{
  Q_OBJECT

public:
  ctkCmdLineModuleExplorerDirectorySettings(ctkCmdLineModuleDirectoryWatcher* directoryWatcher);

  void applySettings();

private:

  ctkCmdLineModuleDirectoryWatcher* DirectoryWatcher;

};

#endif // CTKCMDLINEMODULEEXPLORERDIRECTORYSETTINGS_H
