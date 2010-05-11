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

#ifndef CTKPLUGINFRAMEWORKLISTENERS_H
#define CTKPLUGINFRAMEWORKLISTENERS_H

#include <QObject>

#include <ctkPluginEvent.h>
#include <ctkPluginFrameworkEvent.h>

namespace ctk {

  class PluginFrameworkListeners : public QObject
  {

    Q_OBJECT

  public:

    void frameworkError(Plugin* p, const std::exception& e);

    void emitPluginChanged(const PluginEvent& event);

    void emitFrameworkEvent(const PluginFrameworkEvent& event);

  signals:

    void pluginChanged(const PluginEvent& event);

    void frameworkEvent(const PluginFrameworkEvent& event);

  };

}

#endif // CTKPLUGINFRAMEWORKLISTENERS_H
