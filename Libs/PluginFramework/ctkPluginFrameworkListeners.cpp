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

#include "ctkPluginFrameworkListeners_p.h"


  void ctkPluginFrameworkListeners::frameworkError(ctkPlugin* p, const std::exception& e)
  {
    emit frameworkEvent(ctkPluginFrameworkEvent(ctkPluginFrameworkEvent::ERROR, p, e));
  }

  void ctkPluginFrameworkListeners::emitFrameworkEvent(const ctkPluginFrameworkEvent& event)
  {
    emit frameworkEvent(event);
  }

  void ctkPluginFrameworkListeners::emitPluginChanged(const ctkPluginEvent& event)
  {
    emit pluginChanged(event);

}
