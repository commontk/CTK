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


  class PluginFrameworkContext;
  class PluginFrameworkPrivate;

  /**
   * A %PluginFramework instance. A %PluginFramework is also known as a System %Plugin.
   *
   * <p>
   * %PluginFramework instances are created using a PluginFrameworkFactory. The methods
   * of this class can be used to manage and control the created plugin framework
   * instance.
   *
   * @threadsafe
   */
  class CTK_PLUGINFW_EXPORT PluginFramework : public Plugin
  {

    Q_DECLARE_PRIVATE(PluginFramework)

  public:

    /**
     * Initialize this %PluginFramework. After calling this method, this %PluginFramework
     * must:
     * <ul>
     * <li>Be in the {@link #STARTING} state.</li>
     * <li>Have a valid Plugin Context.</li>
     * <li>Be at start level 0.</li>
     * <li>Have event handling enabled.</li>
     * <li>Have create Plugin objects for all installed plugins.</li>
     * <li>Have registered any framework services.</li>
     * </ul>
     *
     * <p>
     * This %PluginFramework will not actually be started until {@link #start() start}
     * is called.
     *
     * <p>
     * This method does nothing if called when this %PluginFramework is in the
     * {@link #STARTING}, {@link #ACTIVE} or {@link #STOPPING} states.
     *
     * @throws PluginException If this %PluginFramework could not be initialized.
     */
    void init();

    /**
     * Start this %PluginFramework.
     *
     * <p>
     * The following steps are taken to start this %PluginFramework:
     * <ol>
     * <li>If this %PluginFramework is not in the {@link #STARTING} state,
     * {@link #init() initialize} this %PluginFramework.</li>
     * <li>All installed plugins must be started in accordance with each
     * plugin's persistent <i>autostart setting</i>. This means some plugins
     * will not be started, some will be started with <i>lazy activation</i>
     * and some will be started with their <i>declared activation</i> policy.
     * Any exceptions that occur during plugin starting must be wrapped in a
     * {@link PluginException} and then published as a plugin framework event of type
     * {@link PluginFrameworkEvent::ERROR}</li>
     * <li>This %PluinFramework's state is set to {@link #ACTIVE}.</li>
     * <li>A plugin framework event of type {@link PluginFrameworkEvent::STARTED} is fired</li>
     * </ol>
     *
     * @param options Ignored. There are no start options for the %PluginFramework.
     * @throws PluginException If this %PluginFramework could not be started.
     */
    void start(const Plugin::StartOptions& options = 0);

    /**
     * @see Plugin::getHeaders()
     */
    QHash<QString, QString> getHeaders();

    /**
     * @see Plugin::getResourceList()
     */
    QStringList getResourceList(const QString& path) const;

    /**
     * @see Plugin::getResource()
     */
    QByteArray getResource(const QString& path) const;

  protected:

    friend class PluginFrameworkContext;

    PluginFramework(PluginFrameworkContext* fw);

  };


#endif // CTKPLUGINFRAMEWORK_H
