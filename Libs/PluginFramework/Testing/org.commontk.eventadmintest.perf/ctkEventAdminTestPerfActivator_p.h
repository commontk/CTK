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


#ifndef CTKEVENTADMINTESTPERFACTIVATOR_H
#define CTKEVENTADMINTESTPERFACTIVATOR_H

#include <ctkPluginActivator.h>


class ctkEventAdminTestPerfActivator : public QObject,
    public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org_commontk_eventadmintest_perf")
#endif

public:

  ctkEventAdminTestPerfActivator();
  ~ctkEventAdminTestPerfActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:

  QObject* perfTestSuite;
};

#endif // CTKEVENTADMINTESTPERFACTIVATOR_H
