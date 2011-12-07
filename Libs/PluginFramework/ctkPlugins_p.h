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

#ifndef CTKPLUGINS_H
#define CTKPLUGINS_H

#include <QUrl>
#include <QHash>
#include <QReadWriteLock>
#include <QMutex>
#include <QSharedPointer>


// CTK class forward declarations
class ctkPlugin;
class ctkPluginFrameworkContext;
class ctkVersion;
class ctkVersionRange;

/**
 * \ingroup PluginFramework
 *
 * Here we handle all the plugins that are installed in the framework.
 * Also handles load and save of bundle states to a database, so that we
 * can restart the platform.
 */
class ctkPlugins {

private:

  /**
   * Table of all installed plugins in this framework.
   * Key is the plugin location.
   */
  QHash<QString, QSharedPointer<ctkPlugin> > plugins;

  /**
   * Link to framework object.
   */
  ctkPluginFrameworkContext* fwCtx;

  /**
   * Read write lock for protecting the plugins object
   */
  mutable QReadWriteLock pluginsLock;

  /**
   * Lock for protecting this object.
   */
  QMutex objectLock;

  void checkIllegalState() const;

public:

  /**
   * Create a container for all plugins in this framework.
   */
  ctkPlugins(ctkPluginFrameworkContext* fw);


  void clear();


  /**
   * Install a new plugin.
   *
   * @param location The location to be installed
   */
  QSharedPointer<ctkPlugin> install(const QUrl& location, QIODevice* in);


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
   * @return ctkPlugin or null
   *         if the plugin was not found.
   */
  QSharedPointer<ctkPlugin> getPlugin(int id) const;


  /**
   * Get the plugin that has specified plugin location.
   *
   * @param location The location of the plugin to get.
   * @return ctkPlugin or null
   *         if the plugin was not found.
   */
  QSharedPointer<ctkPlugin> getPlugin(const QString& location) const;


  /**
   * Get the plugin that has specified plugin symbolic name and version.
   *
   * @param name The symbolic name of the plugin to get.
   * @param version The plugin version of the plugin to get.
   * @return ctkPlugin or null.
   */
  QSharedPointer<ctkPlugin> getPlugin(const QString& name, const ctkVersion& version) const;


  /**
   * Get all installed plugins.
   *
   * @return A ctkPlugin list with plugins.
   */
  QList<QSharedPointer<ctkPlugin> > getPlugins() const;


  /**
   * Get all plugins that have specified plugin symbolic name.
   *
   * @param name The symbolic name of plugins to get.
   * @return A list of ctkPlugins.
   */
  QList<ctkPlugin*> getPlugins(const QString& name) const;


  /**
   * Get all plugins that have specified plugin symbolic name and
   * version range. Result is sorted in decreasing version order.
   *
   * @param name The symbolic name of plugins to get.
   * @param range ctkVersion range of plugins to get.
   * @return A List of ctkPlugins.
   */
  QList<ctkPlugin*> getPlugins(const QString& name, const ctkVersionRange& range) const;


  /**
   * Get all plugins currently in plugin state ACTIVE.
   *
   * @return A List of ctkPlugins.
   */
  QList<QSharedPointer<ctkPlugin> > getActivePlugins() const;


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
   * @param slist ctkPlugins to start.
   */
  void startPlugins(const QList<ctkPlugin*>& slist) const;


};


#endif // CTKPLUGINS_H
