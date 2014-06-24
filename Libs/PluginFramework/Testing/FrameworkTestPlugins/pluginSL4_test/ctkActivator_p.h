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


#ifndef CTKACTIVATOR_P_H
#define CTKACTIVATOR_P_H

#include <ctkPluginActivator.h>
#include <ctkFooService.h>

class ctkActivator :
  public QObject, public ctkPluginActivator, public ctkFooService
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator ctkFooService)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "pluginSL4_test")
#endif

public:

  void foo();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

}; // ctkActivator

#endif // CTKACTIVATOR_P_H
