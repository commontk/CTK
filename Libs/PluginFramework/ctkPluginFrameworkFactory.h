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

#ifndef CTKPLUGINFRAMEWORKFACTORY_H
#define CTKPLUGINFRAMEWORKFACTORY_H

#include <QHash>
#include <QString>
#include <QVariant>

#include "CTKPluginFrameworkExport.h"

namespace ctk {

  class PluginFramework;
  class PluginFrameworkContext;

  /**
   * A factory for creating PluginFramework instances.
   */
  class CTK_PLUGINFW_EXPORT PluginFrameworkFactory {

  public:

    typedef QHash<QString, QVariant> Properties;

    /**
     * Create a new <code>%PluginFrameworkFactory</code> instance.
     *
     * <p>
     * The Properties map is used to configure the PluginFramework. The
     * plugin framework instance created with #getFramework() must interpret
     * the following properties:
     *
     * <p>
     * <ul><li>PluginConstants::FRAMEWORK_STORAGE</li>
     * </ul>
     *
     * @param initProps The plugin framework configuration properties.
     *        The PluginFramework instance created with #getFramework()
     *        must use some reasonable default configuration if certain
     *        properties are not provided.
     */
    PluginFrameworkFactory(const Properties& initProps = Properties());

    ~PluginFrameworkFactory();

    /**
     * Create a new PluginFramework instance.
     *
     * @return A new, configured PluginFramework instance. The plugin
     *         framework instance must be in the Plugin::INSTALLED state.
     */
    PluginFramework* getFramework();

  private:

    PluginFrameworkContext * const fwCtx;

  };

}

#endif // CTKPLUGINFRAMEWORKFACTORY_H
