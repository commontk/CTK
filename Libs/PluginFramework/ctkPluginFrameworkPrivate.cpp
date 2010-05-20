/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginFrameworkPrivate_p.h"

#include "ctkPluginFramework.h"
#include "ctkPluginConstants.h"

#include "ctkPluginFrameworkContext_p.h"


  PluginFrameworkPrivate::PluginFrameworkPrivate(PluginFramework& qq, PluginFrameworkContext* fw)
    : PluginPrivate(qq, fw, 0, PluginConstants::SYSTEM_PLUGIN_LOCATION,
                    PluginConstants::SYSTEM_PLUGIN_SYMBOLICNAME,
                    // TODO: read version from the manifest resource
                    Version(0, 9, 0))
  {
    systemHeaders.insert(PluginConstants::PLUGIN_SYMBOLICNAME, symbolicName);
    systemHeaders.insert(PluginConstants::PLUGIN_NAME, location);
    systemHeaders.insert(PluginConstants::PLUGIN_VERSION, version.toString());
  }

  void PluginFrameworkPrivate::init()
  {
    this->state = Plugin::STARTING;
    this->fwCtx->init();
  }

  void PluginFrameworkPrivate::initSystemPlugin()
  {
    this->pluginContext = new PluginContext(this);

}
