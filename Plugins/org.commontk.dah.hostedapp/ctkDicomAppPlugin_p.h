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


#ifndef CTKDICOMAPPPLUGIN_P_H
#define CTKDICOMAPPPLUGIN_P_H

// CTK includes
#include <ctkPluginActivator.h>

class ctkDicomAppServer;

class ctkDicomAppPlugin :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org_commontk_dah_hostedapp")
#endif

public:

  ctkDicomAppPlugin();
  virtual ~ctkDicomAppPlugin();

  virtual void start(ctkPluginContext* Context);
  virtual void stop(ctkPluginContext* Context);

  static ctkPluginContext* getPluginContext();


private:

  static ctkPluginContext* Context;

  ctkDicomAppServer* AppServer;
  QObject* HostInterface;

}; // ctkDicomAppPlugin

#endif // CTKDICOMAPPPLUGIN_P_H
