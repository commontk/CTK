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


#ifndef CTKPLUGINFRAMEWORKTESTRUNNER_H
#define CTKPLUGINFRAMEWORKTESTRUNNER_H

#include "ctkPluginFrameworkTestUtilExport.h"

#include <ctkPlugin.h>
#include <ctkPluginFramework_global.h>

class ctkPluginFrameworkTestRunnerPrivate;

class CTK_PLUGINFW_TESTUTIL_EXPORT ctkPluginFrameworkTestRunner
{

public:

  ctkPluginFrameworkTestRunner();
  ~ctkPluginFrameworkTestRunner();

  void addPluginPath(const QString& path, bool install = true);
  void addPlugin(const QString& path, const QString& name);
  void startPluginOnRun(const QString& name, ctkPlugin::StartOptions opts = ctkPlugin::START_ACTIVATION_POLICY);

  void init(const ctkProperties& fwProps = ctkProperties());
  int run(int argc, char** argv);

private:

  Q_DECLARE_PRIVATE(ctkPluginFrameworkTestRunner)

  const QScopedPointer<ctkPluginFrameworkTestRunnerPrivate> d_ptr;
};

#endif // CTKPLUGINFRAMEWORKTESTRUNNER_H
