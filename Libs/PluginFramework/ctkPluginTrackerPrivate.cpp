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


#include "ctkPluginTrackerPrivate.h"

#include "ctkTrackedPlugin_p.h"
#include "ctkPluginTracker.h"

const bool ctkPluginTrackerPrivate::DEBUG = true;

ctkPluginTrackerPrivate::ctkPluginTrackerPrivate(
    ctkPluginTracker* pt, ctkPluginContext* context,
    ctkPlugin::States stateMask, ctkPluginTrackerCustomizer* customizer)
  : context(context), customizer(customizer), mask(stateMask), q_ptr(pt)
{
  this->customizer = customizer ? customizer : q_func();
}

ctkPluginTrackerPrivate::~ctkPluginTrackerPrivate()
{
  if (customizer != q_func())
  {
    delete customizer;
  }
}

QSharedPointer<ctkTrackedPlugin> ctkPluginTrackerPrivate::tracked() const
{
  return trackedPlugin;
}
