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

#include <QCoreApplication>


#include <ctkPluginConstants.h>

#include "ctkPluginFrameworkTestRunner.h"


int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  ctkPluginFrameworkTestRunner testRunner;

  app.setOrganizationName("CTK");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkPluginFrameworkCppTests");

  QString pluginDir;
#ifdef CMAKE_INTDIR
  pluginDir = qApp->applicationDirPath() + "/../test_plugins/" CMAKE_INTDIR "/";
#else
  pluginDir = qApp->applicationDirPath() + "/test_plugins/";
#endif

  testRunner.addPluginPath(pluginDir, false);
  testRunner.addPlugin(pluginDir, "org_commontk_pluginfwtest");
  testRunner.startPluginOnRun("org.commontk.pluginfwtest");

  ctkProperties fwProps;
  fwProps.insert(ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN, ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT);
  fwProps.insert("pluginfw.testDir", pluginDir);

#if defined(Q_CC_GNU) && ((__GNUC__ < 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ < 5)))
  fwProps.insert(ctkPluginConstants::FRAMEWORK_PLUGIN_LOAD_HINTS, QVariant::fromValue<QLibrary::LoadHints>(QLibrary::ExportExternalSymbolsHint));
#endif

  testRunner.init(fwProps);
  return testRunner.run(argc, argv);
}
