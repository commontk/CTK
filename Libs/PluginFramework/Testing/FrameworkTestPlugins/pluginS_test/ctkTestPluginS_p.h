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


#ifndef CTKTESTPLUGINS_P_H
#define CTKTESTPLUGINS_P_H

#include <QObject>

#include "ctkTestPluginSService0.h"
#include "ctkTestPluginSService1.h"
#include "ctkTestPluginSService2.h"
#include "ctkTestPluginSService3.h"

#include <ctkServiceRegistration.h>

class ctkPluginContext;

class ctkTestPluginS : public QObject,
                       public ctkTestPluginSService0,
                       public ctkTestPluginSService1,
                       public ctkTestPluginSService2,
                       public ctkTestPluginSService3
{
  Q_OBJECT
  Q_INTERFACES(ctkTestPluginSService0 ctkTestPluginSService1 ctkTestPluginSService2 ctkTestPluginSService3)

public:

  ctkTestPluginS(ctkPluginContext* pc);
  
public Q_SLOTS:

  void controlService(int service, const QString& operation, long ranking);

private:

  static const QString SERVICE; // = "org.commontk.pluginStest.TestPluginSService"

  ctkPluginContext* pc;
  QList<ctkServiceRegistration> servregs;
  ctkServiceRegistration sreg;
};

#endif // CTKTESTPLUGINS_P_H
