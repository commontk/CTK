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


#include "ctkTestPluginA2Activator_p.h"

#include <ctkPluginContext.h>

#include <QtPlugin>

//----------------------------------------------------------------------------
void ctkTestPluginA2Activator::start(ctkPluginContext* context)
{
  s.reset(new ctkTestPluginA2(context));
}

//----------------------------------------------------------------------------
void ctkTestPluginA2Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  s->unregister();
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(pluginA2_test, ctkTestPluginA2Activator)
#endif
