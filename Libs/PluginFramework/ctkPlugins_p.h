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

#ifndef CTKPLUGINS_H
#define CTKPLUGINS_H

#include <QUrl>
#include <QHash>
#include <QReadWriteLock>



  // CTK class forward declarations
  class Plugin;
  class PluginFrameworkContext;
  class Version;
  class VersionRange;

  /**
   * Here we handle all the plugins that are installed in the framework.
   * Also handles load and save of bundle states to a database, so that we
   * can restart the platform.
   */
  class Plugins {

  private:

    /**
     * Table of all installed plugins in this framework.
     * Key is the plugin location.
     */
    QHash<QString, Plugin*> plugins;

    /**
     * Link to framework object.
     */
    PluginFrameworkContext* fwCtx;

    /**
     * Read write lock for protecting the plugins object
     */
    mutable QReadWriteLock pluginsLock;


  public:

    /**
     * Create a container for all plugins in this framework.
     */
    Plugins(PluginFrameworkContext* fw);


    void clear();


    /**
     * Install a new plugin.
     *
     * @param location The location to be installed
     */
    Plugin* install(const QUrl& location, QIODevice* in);


    /**
     * Remove plugin registration.
     *
     * @param location The location to be removed
     */
    void remove(const QUrl& location);


    /**
     * Get the plugin that has the specified plugin identifier.
     *
     * @param id The identifier of the plugin to get.
     * @return Plugin or null
     *         if the plugin was not found.
     */
    Plugin* getPlugin(int id) const;


    /**
     * Get the plugin that has specified plugin location.
     *
     * @param location The location of the plugin to get.
     * @return Plugin or null
     *         if the plugin was not found.
     */
    Plugin* getPlugin(const QString& location) const;


    /**
     * Get the plugin that has specified plugin symbolic name and version.
     *
     * @param name The symbolic name of the plugin to get.
     * @param version The plugin version of the plugin to get.
     * @return Plugin or null.
     */
    Plugin* getPlugin(const QString& name, const Version& version) const;


    /**
     * Get all installed plugins.
     *
     * @return A Plugin list with plugins.
     */
    QList<Plugin*> getPlugins() const;


    /**
     * Get all plugins that have specified plugin symbolic name.
     *
     * @param name The symbolic name of plugins to get.
     * @return A list of Plugins.
     */
    QList<Plugin*> getPlugins(const QString& name) const;


    /**
     * Get all plugins that have specified plugin symbolic name and
     * version range. Result is sorted in decreasing version order.
     *
     * @param name The symbolic name of plugins to get.
     * @param range Version range of plugins to get.
     * @return A List of Plugins.
     */
    QList<Plugin*> getPlugins(const QString& name, const VersionRange& range) const;


    /**
     * Get all plugins currently in plugin state ACTIVE.
     *
     * @return A List of Plugins.
     */
    QList<Plugin*> getActivePlugins() const;


    /**
     * Try to load any saved framework state.
     * This is done by installing all saved plugins and restoring
     * the saved state for each plugin. This is only
     * intended to be executed during the start of the framework.
     *
     */
    void load();


    /**
     * Start a list of plugins in order
     *
     * @param slist Plugins to start.
     */
    void startPlugins(const QList<Plugin*>& slist) const;


  };


#endif // CTKPLUGINS_H
