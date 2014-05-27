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


#include "ctkActivator_p.h"

#include <QtPlugin>
#include <QDebug>
#include <QStringList>

//----------------------------------------------------------------------------
void ctkActivator::foo()
{
  qDebug() << "pluginSL4: Doing foo";
}

//----------------------------------------------------------------------------
void ctkActivator::start(ctkPluginContext* context)
{
  ctkServiceRegistration registration =
      context->registerService<ctkFooService>(this);
  qDebug() << "pluginSL4: Registered" << registration;
}

//----------------------------------------------------------------------------
void ctkActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  //unregister will be done automagically
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(pluginSL4_test, ctkActivator)
#endif
