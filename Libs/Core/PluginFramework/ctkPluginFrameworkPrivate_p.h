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

#ifndef CTKPLUGINFRAMEWORKPRIVATE_P_H
#define CTKPLUGINFRAMEWORKPRIVATE_P_H

#include "ctkPluginPrivate_p.h"

#include <QMutex>

namespace ctk {

  class PluginFramework;

  class PluginFrameworkPrivate : public PluginPrivate
  {
  public:

    QMutex lock;

    PluginFrameworkPrivate(PluginFramework& qq, PluginFrameworkContextPrivate* fw);

    void init();

    void initSystemPlugin();

  };

}

#endif // CTKPLUGINFRAMEWORKPRIVATE_P_H
