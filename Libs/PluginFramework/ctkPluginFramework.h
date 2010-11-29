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

#ifndef CTKPLUGINFRAMEWORK_H
#define CTKPLUGINFRAMEWORK_H

#include "ctkPlugin.h"


#include "ctkPluginFrameworkExport.h"


class ctkPluginFrameworkContext;
class ctkPluginFrameworkPrivate;

/**
 * A %ctkPluginFramework instance. A %ctkPluginFramework is also known as a System %Plugin.
 *
 * <p>
 * %ctkPluginFramework instances are created using a ctkPluginFrameworkFactory. The methods
 * of this class can be used to manage and control the created plugin framework
 * instance.
 *
 * @threadsafe
 */
class CTK_PLUGINFW_EXPORT ctkPluginFramework : public ctkPlugin
{

  Q_DECLARE_PRIVATE(ctkPluginFramework)
  Q_DISABLE_COPY(ctkPluginFramework)

public:

  /**
   * Initialize this %ctkPluginFramework. After calling this method, this %ctkPluginFramework
   * must:
   * <ul>
   * <li>Be in the {@link #STARTING} state.</li>
   * <li>Have a valid Plugin Context.</li>
   * <li>Be at start level 0.</li>
   * <li>Have event handling enabled.</li>
   * <li>Have create ctkPlugin objects for all installed plugins.</li>
   * <li>Have registered any framework services.</li>
   * </ul>
   *
   * <p>
   * This %ctkPluginFramework will not actually be started until {@link #start() start}
   * is called.
   *
   * <p>
   * This method does nothing if called when this %ctkPluginFramework is in the
   * {@link #STARTING}, {@link #ACTIVE} or {@link #STOPPING} states.
   *
   * @throws ctkPluginException If this %ctkPluginFramework could not be initialized.
   */
  void init();

  /**
   * Start this %ctkPluginFramework.
   *
   * <p>
   * The following steps are taken to start this %ctkPluginFramework:
   * <ol>
   * <li>If this %ctkPluginFramework is not in the {@link #STARTING} state,
   * {@link #init() initialize} this %ctkPluginFramework.</li>
   * <li>All installed plugins must be started in accordance with each
   * plugin's persistent <i>autostart setting</i>. This means some plugins
   * will not be started, some will be started with <i>lazy activation</i>
   * and some will be started with their <i>declared activation</i> policy.
   * Any exceptions that occur during plugin starting must be wrapped in a
   * {@link ctkPluginException} and then published as a plugin framework event of type
   * {@link ctkPluginFrameworkEvent::ERROR}</li>
   * <li>This %PluinFramework's state is set to {@link #ACTIVE}.</li>
   * <li>A plugin framework event of type {@link ctkPluginFrameworkEvent::STARTED} is fired</li>
   * </ol>
   *
   * @param options Ignored. There are no start options for the %ctkPluginFramework.
   * @throws ctkPluginException If this %ctkPluginFramework could not be started.
   */
  void start(const ctkPlugin::StartOptions& options = 0);

  /**
   * @see ctkPlugin::getHeaders()
   */
  QHash<QString, QString> getHeaders();

  /**
   * @see ctkPlugin::getResourceList()
   */
  QStringList getResourceList(const QString& path) const;

  /**
   * @see ctkPlugin::getResource()
   */
  QByteArray getResource(const QString& path) const;

protected:

  friend class ctkPluginFrameworkContext;

  ctkPluginFramework();

};


#endif // CTKPLUGINFRAMEWORK_H
