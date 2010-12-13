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

#include "ctkPluginFrameworkPrivate_p.h"

#include "ctkPluginFramework.h"
#include "ctkPluginConstants.h"
#include "ctkPluginContext.h"
#include "ctkPluginContext_p.h"
#include "ctkPluginFrameworkContext_p.h"


ctkPluginFrameworkPrivate::ctkPluginFrameworkPrivate(QWeakPointer<ctkPlugin> qq, ctkPluginFrameworkContext* fw)
  : ctkPluginPrivate(qq, fw, 0, ctkPluginConstants::SYSTEM_PLUGIN_LOCATION,
                     ctkPluginConstants::SYSTEM_PLUGIN_SYMBOLICNAME,
                     // TODO: read version from the manifest resource
                     ctkVersion(0, 9, 0))
{
  systemHeaders.insert(ctkPluginConstants::PLUGIN_SYMBOLICNAME, symbolicName);
  systemHeaders.insert(ctkPluginConstants::PLUGIN_NAME, location);
  systemHeaders.insert(ctkPluginConstants::PLUGIN_VERSION, version.toString());
}

void ctkPluginFrameworkPrivate::init()
{
  this->state = ctkPlugin::STARTING;
  this->fwCtx->init();
}

void ctkPluginFrameworkPrivate::initSystemPlugin()
{
  this->pluginContext.reset(new ctkPluginContext(this));
}

void ctkPluginFrameworkPrivate::uninitSystemPlugin()
{
  this->pluginContext->d_func()->invalidate();
}

