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

#include "ctkPluginFrameworkEvent.h"


  PluginFrameworkEvent::PluginFrameworkEvent()
    : d(0)
  {

  }

  PluginFrameworkEvent::PluginFrameworkEvent(Type type, Plugin* plugin, const std::exception& fwException)
    : d(new PluginFrameworkEventData(type, plugin, fwException.what()))
  {

  }

  PluginFrameworkEvent::PluginFrameworkEvent(Type type, Plugin* plugin)
    : d(new PluginFrameworkEventData(type, plugin, QString()))
  {

  }

  PluginFrameworkEvent::PluginFrameworkEvent(const PluginFrameworkEvent& other)
    : d(other.d)
  {

  }

  QString PluginFrameworkEvent::getErrorString() const
  {
    return d->errorString;
  }

  Plugin* PluginFrameworkEvent::getPlugin() const
  {
    return d->plugin;
  }

  PluginFrameworkEvent::Type PluginFrameworkEvent::getType() const
  {
    return d->type;

}
