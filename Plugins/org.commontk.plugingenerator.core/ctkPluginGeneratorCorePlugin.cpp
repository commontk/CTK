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


#include "ctkPluginGeneratorCorePlugin_p.h"

#include "ctkPluginGeneratorCodeModel.h"

#include <QtPlugin>

ctkPluginGeneratorCorePlugin* ctkPluginGeneratorCorePlugin::instance = 0;

void ctkPluginGeneratorCorePlugin::start(ctkPluginContext* context)
{
  pluginContext = context;
  instance = this;

  codeModel = new ctkPluginGeneratorCodeModel();
  context->registerService(QStringList("ctkPluginGeneratorCodeModel"), codeModel);
}

void ctkPluginGeneratorCorePlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete codeModel;
  instance = 0;
}

ctkPluginContext* ctkPluginGeneratorCorePlugin::getContext() const
{
  return pluginContext;
}

ctkPluginGeneratorCodeModel* ctkPluginGeneratorCorePlugin::getCodeModel() const
{
  return codeModel;
}

ctkPluginGeneratorCorePlugin* ctkPluginGeneratorCorePlugin::getInstance()
{
  return instance;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_plugingenerator_core, ctkPluginGeneratorCorePlugin)
#endif
