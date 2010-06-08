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


class ctkPluginFrameworkListeners : public QObject
{

  Q_OBJECT

public:

  void frameworkError(ctkPlugin* p, const std::exception& e);

  void emitPluginChanged(const ctkPluginEvent& event);

  void emitFrameworkEvent(const ctkPluginFrameworkEvent& event);

signals:

  void pluginChanged(const ctkPluginEvent& event);

  void frameworkEvent(const ctkPluginFrameworkEvent& event);

};


#endif // CTKPLUGINFRAMEWORKLISTENERS_H
