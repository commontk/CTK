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

#include "ctkPluginFramework.h"

#include "ctkPluginFrameworkPrivate_p.h"
#include "ctkPluginPrivate_p.h"
#include "ctkPluginFrameworkContextPrivate_p.h"


namespace ctk {

  PluginFramework::PluginFramework(PluginFrameworkContextPrivate* fw)
    : Plugin(*new PluginFrameworkPrivate(*this, fw))
  {

  }

  void PluginFramework::init()
  {
    Q_D(PluginFramework);

    QMutexLocker sync(&d->lock);
    // waitOnActivation(d->lock, "Framework.ini", true);
    switch (d->state)
    {
    case Plugin::INSTALLED:
    case Plugin::RESOLVED:
      break;
    case Plugin::STARTING:
    case Plugin::ACTIVE:
      return;
    default:
      throw std::logic_error("INTERNAL ERROR, Illegal state");
    }
    d->init();
  }

  void waitForStop(int timeout)
  {
    // TODO implement
  }

}
