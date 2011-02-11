/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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
#include <QSharedPointer>

#include "ctkPluginFrameworkExport.h"

#include "ctkPluginFramework_global.h"

class ctkPluginFramework;
class ctkPluginFrameworkContext;

/**
 * \ingroup PluginFramework
 * A factory for creating ctkPluginFramework instances.
 */
class CTK_PLUGINFW_EXPORT ctkPluginFrameworkFactory
{

public:

  /**
   * Create a new <code>%ctkPluginFrameworkFactory</code> instance.
   *
   * <p>
   * The ctkProperties map is used to configure the ctkPluginFramework. The
   * plugin framework instance created with #getFramework() must interpret
   * the following properties:
   *
   * <p>
   * <ul><li>ctkPluginConstants::FRAMEWORK_STORAGE</li>
   * </ul>
   *
   * @param initProps The plugin framework configuration properties.
   *        The ctkPluginFramework instance created with #getFramework()
   *        must use some reasonable default configuration if certain
   *        properties are not provided.
   */
  ctkPluginFrameworkFactory(const ctkProperties& initProps = ctkProperties());

  ~ctkPluginFrameworkFactory();

  /**
   * Create a new ctkPluginFramework instance.
   *
   * @return A new, configured ctkPluginFramework instance. The plugin
   *         framework instance must be in the ctkPlugin::INSTALLED state.
   */
  QSharedPointer<ctkPluginFramework> getFramework();

private:

  ctkPluginFrameworkContext * const fwCtx;

};


#endif // CTKPLUGINFRAMEWORKFACTORY_H
