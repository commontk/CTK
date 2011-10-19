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


#include "ctkTestPluginS_p.h"

#include <ctkPluginContext.h>

#include <QStringList>

const QString ctkTestPluginS::SERVICE = "org.commontk.pluginStest.TestPluginSService";

//----------------------------------------------------------------------------
ctkTestPluginS::ctkTestPluginS(ctkPluginContext* pc)
  : pc(pc)
{
  for(int i = 0; i <= 3; ++i)
  {
    servregs.push_back(ctkServiceRegistration());
  }
  sreg = pc->registerService<ctkTestPluginSService0>(this);
}

//----------------------------------------------------------------------------
void ctkTestPluginS::controlService(int offset, const QString& operation, long ranking)
{
  if (0 <= offset && offset <= 3)
  {
    if (operation == "register")
    {
      if (!servregs[offset])
      {
        QString servicename = SERVICE + QString::number(offset);
        ctkProperties props;
        props.insert(ctkPluginConstants::SERVICE_RANKING, QVariant::fromValue(ranking));
        servregs[offset] = pc->registerService(QStringList(servicename), this, props);
      }
    }
    if (operation == "unregister")
    {
      if (servregs[offset])
      {
        ctkServiceRegistration sr1 = servregs[offset];
        sr1.unregister();
        servregs[offset] = 0;
      }
    }
  }
}
