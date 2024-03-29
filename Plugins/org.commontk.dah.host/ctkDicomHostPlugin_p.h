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


#ifndef CTKDICOMHOSTPLUGIN_P_H
#define CTKDICOMHOSTPLUGIN_P_H

#include <ctkPluginActivator.h>

class ctkDicomHostPlugin :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_commontk_dah_host")

public:

  ctkDicomHostPlugin();
  virtual ~ctkDicomHostPlugin();

  virtual void start(ctkPluginContext* context);
  virtual void stop(ctkPluginContext* context);

  static ctkDicomHostPlugin* getInstance();

  ctkPluginContext* getPluginContext() const;


private:

  static ctkDicomHostPlugin* Instance;
  ctkPluginContext* Context;


}; // ctkDicomHostPlugin

#endif // CTKDICOMHOSTPLUGIN_P_H
