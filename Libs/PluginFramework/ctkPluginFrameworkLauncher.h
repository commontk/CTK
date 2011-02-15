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

#ifndef CTKPLUGINFRAMEWORKLAUNCHER_H
#define CTKPLUGINFRAMEWORKLAUNCHER_H

#include <QString>
#include <QScopedPointer>

#include <ctkPluginFrameworkExport.h>
#include "ctkPlugin.h"
#include "ctkPluginFramework_global.h"

class ctkPluginFramework;
class ctkPluginFrameworkLauncherPrivate;

/**
 * \ingroup PluginFramework
 *
 * Sets up and starts the CTK Plugin Framework.
 *
 * This is a convenience class to start an instance of the CTK Plugin
 * Framework and to install and start plugins.
 */
class CTK_PLUGINFW_EXPORT ctkPluginFrameworkLauncher
{
public:

  /**
   * Specify the set of framework properties to be used when
   * initializing the Plugin Framework.
   *
   * <p>
   * If the framework has already been initialized by a call
   * to #install(const QString&) or #start, the
   * new properties do not have any effect until the framework
   * is restarted.
   *
   * \param props The initial Plugin Framework properties.
   */
  static void setFrameworkProperties(const ctkProperties& props);

  /**
   * Install the plugin with the given symbolic name in the Plugin
   * Framework.
   *
   * <p>
   * This method instantiates and initializes an instance of a
   * ctkPluginFramework object, if there is no framework already
   * initialized.
   *
   * <p>
   * The plugin is searched in the paths previously given by calls
   * to #addSearchPath(const QString&, bool) and in the default
   * CTK plugin installation path.
   *
   * \param symbolicName The symbolic name of the plugin to install.
   * \return <code>true</code> if the plugin was found and successfully
   *         installed, <code>false</code> otherwise.
   */
  static bool install(const QString& symbolicName);

  /**
   * Start the CTK Plugin Framework and/or the plugin with the given
   * symbolic name.
   *
   * <p>
   * This method creates and initializes the CTK Plugin Framework if it
   * has not been previously initialized and starts it. It then optionally
   * starts the plugin with the given symbolic name and start options.
   *
   * <p>
   * If a symbolic name is given, the plugin is searched in the paths previously given by calls
   * to #addSearchPath(const QString&, bool) and in the default
   * CTK plugin installation path.
   *
   * \param symbolicName The symbolic name of the plugin to install.
   * \param options The options used to start the plugin.
   * \return <code>true</code> if the plugin was found and successfully
   *         installed, <code>false</code> otherwise.
   *
   * \see ctkPlugin::StartOption
   */
  static bool start(const QString& symbolicName = QString(), ctkPlugin::StartOptions options = ctkPlugin::START_ACTIVATION_POLICY);

  /**
   * Get the plugin context for the Plugin Framework.
   *
   * \return The context associated to the Plugin Framework, or <code>null</code>
   *         if the framework has not been initialized yet.
   */
  static ctkPluginContext* getPluginContext();

  /**
   * Get the Plugin Framework:
   *
   * \return The initialized Plugin Framework, or <code>null</code> if the
   *         framework has not been initialized yet.
   */
  static QSharedPointer<ctkPluginFramework> getPluginFramework();

  /**
   * Append a path to the PATH environment variable.
   *
   * <p>
   * This method appends the given path to the PATH environment variable.
   * It does nothing on non-Windows platforms.
   *
   * \param path The path to be appended to PATH
   */
  static void appendPathEnv(const QString& path);

  /**
   * Add a path to the list of search paths for plugins.
   *
   * When calling #install(const QString&), #start, or
   * #getPluginPath(const QString&), the plugin is searched in the
   * paths given as arguments to this method. The least recently added
   * path is searched first.
   *
   * \param searchPath The path to add.
   * \param addToPathEnv If <code>true</code>, add the given path to the
   *        PATH environment variable, using #appendPathEnv(const QString&).
   */
  static void addSearchPath(const QString& searchPath, bool addToPathEnv = true);

  /**
   * Get the full path (including the file name) to the plugin with the
   * given symbolic name.
   *
   * <p>
   * The paths given by calls to #addSearchPath(const QString&, bool) are searched
   * for a shared library with a base name equaling <code>symbolicName</code>.
   *
   * \param symbolicName The symbolic name of the plugin to find.
   * \return The full path (including the file name) to the plugin (shared library)
   *         or a null QString if the plugin was not found.
   */
  static QString getPluginPath(const QString& symbolicName);

private:

  static const QScopedPointer<ctkPluginFrameworkLauncherPrivate> d;

  Q_DISABLE_COPY(ctkPluginFrameworkLauncher)
};

#endif // CTKPLUGINFRAMEWORKLAUNCHER_H
