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


#ifndef CTKTESTPLUGINA2_P_H
#define CTKTESTPLUGINA2_P_H

#include <QObject>

#include "ctkTestPluginA2Service.h"

#include <ctkServiceRegistration.h>

class ctkPluginContext;
class ctkServiceRegistration;

class ctkTestPluginA2 : public QObject,
                       public ctkTestPluginA2Service
{
  Q_OBJECT
  Q_INTERFACES(ctkTestPluginA2Service)

public:
  ctkTestPluginA2(ctkPluginContext* pc);

  void unregister();

private:

  ctkServiceRegistration sr;
};

#endif // CTKTESTPLUGINA2_P_H
