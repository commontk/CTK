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

#ifndef CTKPLUGINFRAMEWORK_H
#define CTKPLUGINFRAMEWORK_H

#include "ctkPlugin.h"


#include "CTKPluginFrameworkExport.h"

namespace ctk {

  class PluginFrameworkContextPrivate;
  class PluginFrameworkPrivate;

  class CTK_PLUGINFW_EXPORT PluginFramework : public Plugin
  {

    Q_DECLARE_PRIVATE(PluginFramework)

  public:

    /**
     * Initialize this framework.
     */
    void init();

    // TODO return info about the reason why this
    // method returned
    void waitForStop(int timeout);

    QStringList getResourceList(const QString& path) const;

    QByteArray getResource(const QString& path) const;

  protected:

    friend class PluginFrameworkContextPrivate;

    PluginFramework(PluginFrameworkContextPrivate* fw);

  };

}

#endif // CTKPLUGINFRAMEWORK_H
