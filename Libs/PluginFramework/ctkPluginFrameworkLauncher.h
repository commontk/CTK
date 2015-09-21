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

class QRunnable;

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

  // System properties
  static const QString PROP_USER_HOME; // = "user.home";
  static const QString PROP_USER_DIR; // = "user.dir";

  // Framework properties
  static const QString PROP_PLUGINS; // = "ctk.plugins";
  static const QString PROP_PLUGINS_START_OPTIONS; // = "ctk.plugins.startOptions";
  static const QString PROP_DEBUG; // = "ctk.debug";
  static const QString PROP_DEV; // = "ctk.dev";
  static const QString PROP_CONSOLE; // = "ctk.console";
  static const QString PROP_OS; // = "ctk.os";
  static const QString PROP_ARCH; // = "ctk.arch";

  static const QString PROP_NOSHUTDOWN; // = "ctk.noShutdown";
  static const QString PROP_IGNOREAPP; // = "ctk.ignoreApp";

  static const QString PROP_INSTALL_AREA; // = "ctk.install.area";
  static const QString PROP_CONFIG_AREA; // = "ctk.configuration.area";
  static const QString PROP_SHARED_CONFIG_AREA; // = "ctk.sharedConfiguration.area";
  static const QString PROP_INSTANCE_AREA; // = "ctk.instance.area";
  static const QString PROP_USER_AREA; // = "ctk.user.area";
  static const QString PROP_HOME_LOCATION_AREA; // = "ctk.home.location";


  static const QString PROP_CONFIG_AREA_DEFAULT; // = "ctk.configuration.area.default";
  static const QString PROP_INSTANCE_AREA_DEFAULT; // = "ctk.instance.area.default";
  static const QString PROP_USER_AREA_DEFAULT; // = "ctk.user.area.default";

  static const QString PROP_EXITCODE; // = "ctk.exitcode";
  static const QString PROP_EXITDATA; // = "ctk.exitdata";
  static const QString PROP_CONSOLE_LOG; // = "ctk.consoleLog";

  static const QString PROP_ALLOW_APPRELAUNCH; // = "ctk.allowAppRelaunch";
  static const QString PROP_APPLICATION_LAUNCHDEFAULT; // = "ctk.application.launchDefault";

  static const QString PROP_OSGI_RELAUNCH; // = "ctk.pluginfw.relaunch";


  /**
   * Specify the set of framework properties to be used when
   * initializing the Plugin Framework.
   *
   * <p>
   * If the framework has already been initialized by a call
   * to #install or #start, the
   * new properties do not have any effect until the framework
   * is restarted.
   *
   * \param props The initial Plugin Framework properties.
   */
  static void setFrameworkProperties(const ctkProperties& props);

  /**
   * Launches the platform and runs a single application. The application is either identified
   * in the given arguments (e.g., -application &lt;app id&gt;) or in the <code>eclipse.application</code>
   * System property.  This convenience method starts
   * up the platform, runs the indicated application, and then shuts down the
   * platform. The platform must not be running already.
   *
   * @param endSplashHandler the block of code to run to tear down the splash
   * 	screen or <code>null</code> if no tear down is required
   * @param argument the argument passed to the application. Default is QVariant().
   * @return the result of running the application
   * @throws Exception if anything goes wrong
   */
  static QVariant run(QRunnable* endSplashHandler = NULL, const QVariant& argument = QVariant());

  /**
   * Runs the application for which the platform was started. The platform
   * must be running.
   * <p>
   * The given argument is passed to the application being run.  If it is <code>null</code>
   * then the command line arguments used in starting the platform, and not consumed
   * by the platform code, are passed to the application as a <code>String[]</code>.
   * </p>
   * @param argument the argument passed to the application. May be <code>null</code>
   * @return the result of running the application
   * @throws std::exception if anything goes wrong
   */
  static QVariant run(const QVariant& argument);


  /**
   * Starts the platform and sets it up to run a single application. The application is either identified
   * in the given arguments (e.g., -application &lt;app id&gt;) or in the <code>eclipse.application</code>
   * System property.  The platform must not be running already.
   * <p>
   * The given runnable (if not <code>null</code>) is used to tear down the splash screen if required.
   * </p>
   * @param args the arguments passed to the application
   * @return BundleContext the context of the system bundle
   * @throws Exception if anything goes wrong
   */
  static ctkPluginContext* startup(QRunnable* endSplashHandler);


  /**
   * Shuts down the Platform. The state of the Platform is not automatically
   * saved before shutting down.
   * <p>
   * On return, the Platform will no longer be running (but could be re-launched
   * with another call to startup). If relaunching, care must be taken to reinitialize
   * any System properties which the platform uses (e.g., osgi.instance.area) as
   * some policies in the platform do not allow resetting of such properties on
   * subsequent runs.
   * </p><p>
   * Any objects handed out by running Platform,
   * including Platform runnables obtained via getRunnable, will be
   * permanently invalid. The effects of attempting to invoke methods
   * on invalid objects is undefined.
   * </p>
   * @throws std::exception if anything goes wrong
   */
  static void shutdown();


  /**
   * Install the plugin with the given symbolic name using the provided
   * <code>ctkPluginContext</code>.
   *
   * <p>
   * This method instantiates and initializes an instance of a
   * ctkPluginFramework object, if no plugin context is provided and
   * if there is no framework already initialized.
   *
   * <p>
   * The plugin is searched in the paths previously given by calls
   * to #addSearchPath(const QString&, bool) and in the default
   * CTK plugin installation path.
   *
   * \param symbolicName The symbolic name of the plugin to install.
   * \param context The plugin context used for installing the plugin.
   * \return The plugin id if the plugin was found and successfully
   *         installed, <code>-1</code> otherwise.
   */
  static long install(const QString& symbolicName, ctkPluginContext* context = 0);

  /**
   * This method instantiates, initializes, and starts an instance of a
   * ctkPluginFramework object, if no plugin context is provided and
   * if there is no framework already running. It then installs and
   * starts the plugin with the given symbolic name (if not empty).
   *
   * <p>
   * If a symbolic name is given, the plugin is searched in the paths previously given by calls
   * to #addSearchPath(const QString&, bool) and in the default
   * CTK plugin installation path and is started using the given <code>options</code>.
   *
   * <p>
   * If a plugin context is provided, this context is used to install the plugin,
   * otherwise the Plugin Framework context is used.
   *
   * \param symbolicName The symbolic name of the plugin to install.
   * \param options The options used to start the plugin.
   * \param context The plugin context to use for installing the plugin.
   * \return <code>true</code> if the plugin was found and successfully
   *         installed, <code>false</code> otherwise.
   *
   * \see ctkPlugin::StartOptions
   */
  static bool start(const QString& symbolicName = QString(),
                    ctkPlugin::StartOptions options = ctkPlugin::START_ACTIVATION_POLICY,
                    ctkPluginContext* context = 0);

  /**
   * This method either stops the plug-in with the given <code>symbolicName</code> using
   * the supplied stop options <code>options</code>
   * or the complete framework (if <code>symbolicName</code> is empty).
   *
   * <p>
   * If a plugin context is provided, this context is used to find the plug-in,
   * otherwise the Plugin Framework context is used.
   *
   * \param symbolicName The symbolic name of the plugin to stop.
   * \param options The options used to stop the plugin.
   * \param context The plugin context to use for finding the plugin.
   * \return <code>true</code> if the plugin was found and successfully
   *         stopped or the complete framework was successfully stopped,
   *         <code>false</code> otherwise.
   *
   * \see ctkPlugin::StopOptions
   */
  static bool stop(const QString& symbolicName = QString(),
                    ctkPlugin::StopOptions options = 0, ctkPluginContext* context = 0);

  /**
   * Resolve the given plug-in.
   *
   * This method tries to resolv the given plug-in. In case of an error,
   * a framework event of type ctkPluginFrameworkEvent::PLUGIN_ERROR is send.
   *
   * @param plugin The plug-in to be resolved.
   */
  static void resolve(const QSharedPointer<ctkPlugin>& plugin);

  /**
   * Resolve all currently installed plug-ins.
   *
   * In case of an error, a framework event of type
   * ctkPluginFrameworkEvent::PLUGIN_ERROR is send.
   */
  static void resolve();

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
   * This is a utility method to append a path to the PATH environment variable
   * on Windows platforms.
   *
   * <p>
   * This method does nothing on non-Windows platforms.
   *
   * \param path The path to be appended to PATH
   */
  static void appendPathEnv(const QString& path);

  /**
   * Add a path to the list of search paths for plugins.
   *
   * When calling #install(const QString&, ctkPluginContext*), #start, or
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

  /**
   * Get a list of symbolic names for the plugins in <code>searchPath</code>.
   *
   * <p>
   * The symbolic names are deduced from the shared libraries found in
   * <code>searchPath</code>, which may not represent loadable CTK plugins.
   *
   * \param searchPath The path to look for plugins.
   * \return A list of potential plugin symbolic names.
   */
  static QStringList getPluginSymbolicNames(const QString& searchPath);

private:

  static const QScopedPointer<ctkPluginFrameworkLauncherPrivate> d;

  Q_DISABLE_COPY(ctkPluginFrameworkLauncher)
};

#endif // CTKPLUGINFRAMEWORKLAUNCHER_H
