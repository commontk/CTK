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


#include <QCoreApplication>

#include <ctkConfig.h>
#include <ctkPluginConstants.h>

#include <Testing/Cpp/ctkPluginFrameworkTestRunner.h>


int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  ctkPluginFrameworkTestRunner testRunner;

  app.setOrganizationName("CTK");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkEventAdminImplCppTests");

  QString pluginDir;
#ifdef CMAKE_INTDIR
  pluginDir = CTK_PLUGIN_DIR CMAKE_INTDIR "/";
#else
  pluginDir = CTK_PLUGIN_DIR;
#endif

  QString testpluginDir;
#ifdef CMAKE_INTDIR
  testpluginDir = qApp->applicationDirPath() + "/../test_plugins/" CMAKE_INTDIR "/";
#else
  testpluginDir = qApp->applicationDirPath() + "/test_plugins/";
#endif

  testRunner.addPluginPath(pluginDir, false);
  testRunner.addPlugin(testpluginDir, "org_commontk_eventadmintest_perf");
  testRunner.addPlugin(pluginDir, "org_commontk_eventadmin");
  testRunner.addPlugin(pluginDir, "org_commontk_log");
  testRunner.startPluginOnRun("org.commontk.eventadmintest.perf");

  ctkProperties fwProps;
  fwProps.insert(ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN, ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT);
  fwProps.insert("pluginfw.testDir", testpluginDir);
  fwProps.insert("event.impl", "org.commontk.eventadmin");

  fwProps.insert("org.commontk.eventadmin.ThreadPoolSize", 10);

  testRunner.init(fwProps);
  return testRunner.run(argc, argv);
}
