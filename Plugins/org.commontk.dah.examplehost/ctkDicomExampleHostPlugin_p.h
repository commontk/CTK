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


#ifndef CTKDICOMEXAMPLEHOSTPLUGIN_P_H
#define CTKDICOMEXAMPLEHOSTPLUGIN_P_H

#include <ctkPluginActivator.h>

class ctkDicomExampleHostPlugin :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  ctkDicomExampleHostPlugin();
  ~ctkDicomExampleHostPlugin();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  static ctkDicomExampleHostPlugin* getInstance();

  ctkPluginContext* getPluginContext() const;


private:

  static ctkDicomExampleHostPlugin* instance;
  ctkPluginContext* context;


}; // ctkDicomExampleHostPlugin

#endif // CTKDICOMEXAMPLEHOSTPLUGIN_P_H
