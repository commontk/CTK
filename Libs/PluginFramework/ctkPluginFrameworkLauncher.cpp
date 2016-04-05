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

#include "ctkPluginFrameworkLauncher.h"
#include "ctkPluginFrameworkFactory.h"
#include "ctkPluginFrameworkProperties_p.h"
#include "ctkPluginFramework.h"
#include "ctkPluginContext.h"
#include "ctkPluginException.h"
#include "ctkPlugin_p.h"
#include "ctkDefaultApplicationLauncher_p.h"
#include "ctkLocationManager_p.h"
#include "ctkBasicLocation_p.h"

#include <ctkConfig.h>

#include <QStringList>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QRunnable>
#include <QSettings>
#include <QProcessEnvironment>

#ifdef _WIN32
#include <windows.h>
#include <cstdlib>
#endif // _WIN32


const QString ctkPluginFrameworkLauncher::PROP_USER_HOME = "user.home";
const QString ctkPluginFrameworkLauncher::PROP_USER_DIR = "user.dir";

// Framework properties
const QString ctkPluginFrameworkLauncher::PROP_PLUGINS = "ctk.plugins";
const QString ctkPluginFrameworkLauncher::PROP_PLUGINS_START_OPTIONS = "ctk.plugins.startOptions";
const QString ctkPluginFrameworkLauncher::PROP_DEBUG = "ctk.debug";
const QString ctkPluginFrameworkLauncher::PROP_DEV = "ctk.dev";
const QString ctkPluginFrameworkLauncher::PROP_CONSOLE = "ctk.console";
const QString ctkPluginFrameworkLauncher::PROP_OS = "ctk.os";
const QString ctkPluginFrameworkLauncher::PROP_ARCH = "ctk.arch";

const QString ctkPluginFrameworkLauncher::PROP_NOSHUTDOWN = "ctk.noShutdown";
const QString ctkPluginFrameworkLauncher::PROP_IGNOREAPP = "ctk.ignoreApp";

const QString ctkPluginFrameworkLauncher::PROP_INSTALL_AREA = "ctk.install.area";
const QString ctkPluginFrameworkLauncher::PROP_CONFIG_AREA = "ctk.configuration.area";
const QString ctkPluginFrameworkLauncher::PROP_SHARED_CONFIG_AREA = "ctk.sharedConfiguration.area";
const QString ctkPluginFrameworkLauncher::PROP_INSTANCE_AREA = "ctk.instance.area";
const QString ctkPluginFrameworkLauncher::PROP_USER_AREA = "ctk.user.area";
const QString ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA = "ctk.home.location";


const QString ctkPluginFrameworkLauncher::PROP_CONFIG_AREA_DEFAULT = "ctk.configuration.area.default";
const QString ctkPluginFrameworkLauncher::PROP_INSTANCE_AREA_DEFAULT = "ctk.instance.area.default";
const QString ctkPluginFrameworkLauncher::PROP_USER_AREA_DEFAULT = "ctk.user.area.default";

const QString ctkPluginFrameworkLauncher::PROP_EXITCODE = "ctk.exitcode";
const QString ctkPluginFrameworkLauncher::PROP_EXITDATA = "ctk.exitdata";
const QString ctkPluginFrameworkLauncher::PROP_CONSOLE_LOG = "ctk.consoleLog";

const QString ctkPluginFrameworkLauncher::PROP_ALLOW_APPRELAUNCH = "ctk.allowAppRelaunch";
const QString ctkPluginFrameworkLauncher::PROP_APPLICATION_LAUNCHDEFAULT = "ctk.application.launchDefault";

const QString ctkPluginFrameworkLauncher::PROP_OSGI_RELAUNCH = "ctk.pluginfw.relaunch";

static const QString PROP_FORCED_RESTART = "ctk.forcedRestart";

class ctkPluginFrameworkLauncherPrivate
{
public:

  //----------------------------------------------------------------------------
  ctkPluginFrameworkLauncherPrivate()
    : fwFactory(0)
    , running(false)
    , processEnv(QProcessEnvironment::systemEnvironment())
  {
#ifdef CMAKE_INTDIR
    QString pluginPath = CTK_PLUGIN_DIR CMAKE_INTDIR "/";
#else
    QString pluginPath = CTK_PLUGIN_DIR;
#endif

    pluginSearchPaths.append(pluginPath);

    pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
  }

  //----------------------------------------------------------------------------
  bool isForcedRestart() const
  {
    return ctkPluginFrameworkProperties::getProperty(PROP_FORCED_RESTART).toBool();
  }

  //----------------------------------------------------------------------------
  void loadConfigurationInfo()
  {
    ctkBasicLocation* configArea = ctkLocationManager::getConfigurationLocation();
    if (configArea == NULL)
    {
      return;
    }

    QUrl location(configArea->getUrl().toString() + ctkLocationManager::CONFIG_FILE);
    mergeProperties(ctkPluginFrameworkProperties::getProperties(), loadProperties(location));
  }

  //----------------------------------------------------------------------------
  void mergeProperties(ctkProperties& destination, const ctkProperties& source)
  {
      for (ctkProperties::const_iterator iter = source.begin(); iter != source.end(); ++iter)
      {
        if (!destination.contains(iter.key()))
        {
          destination.insert(iter.key(), iter.value());
        }
      }
  }

  //----------------------------------------------------------------------------
  ctkProperties loadProperties(const QUrl& location)
  {
    ctkProperties result;
    if (!location.isValid() || !QFileInfo(location.toLocalFile()).exists())
    {
      return result;
    }
    QSettings iniProps(location.toLocalFile(), QSettings::IniFormat);
    foreach (const QString& key, iniProps.allKeys())
    {
      result.insert(key, iniProps.value(key));
    }

    return substituteVars(result);
  }

  //----------------------------------------------------------------------------
  ctkProperties& substituteVars(ctkProperties& result)
  {
    for (ctkProperties::iterator iter = result.begin(); iter != result.end(); ++iter)
    {
      if (iter.value().type() == QVariant::String)
      {
        iter.value() = substituteVars(iter.value().toString());
      }
    }
    return result;
  }

  //----------------------------------------------------------------------------
  QString substituteVars(const QString& path)
  {
    QString buf;
    bool varStarted = false; // indicates we are processing a var subtitute
    QString var; // the current var key
    for (QString::const_iterator iter = path.begin(); iter != path.end(); ++iter)
    {
      QChar tok = *iter;
      if (tok == '$')
      {
        if (!varStarted)
        {
          varStarted = true; // we found the start of a var
          var.clear();
        }
        else
        {
          // we have found the end of a var
          QVariant prop;
          // get the value of the var from system properties
          if (!var.isEmpty())
          {
            prop = ctkPluginFrameworkProperties::getProperty(var);
          }
          if (prop.isNull() && processEnv.contains(var))
          {
            prop = processEnv.value(var);
          }
          if (!prop.isNull())
          {
            // found a value; use it
            buf.append(prop.toString());
          }
          else
          {
            // could not find a value append the var name w/o delims
            buf.append(var);
          }
          varStarted = false;
          var.clear();
        }
      }
      else
      {
        if (!varStarted)
        {
          buf.append(tok); // the token is not part of a var
        }
        else
        {
          var.append(tok); // the token is part of the var key; save the key to process when we find the end token
        }
      }
    }

    if (!var.isEmpty())
    {
      // found a case of $var at the end of the path with no trailing $; just append it as is.
      buf.append('$').append(var);
    }
    return buf;
  }

  //----------------------------------------------------------------------------
  QSharedPointer<ctkPlugin> install(const QUrl& pluginPath, ctkPluginContext* context)
  {
    try
    {
      return context->installPlugin(pluginPath);
    }
    catch (const ctkPluginException& exc)
    {
      qWarning() << "Failed to install plugin " << pluginPath << ":" << exc.printStackTrace();
      return QSharedPointer<ctkPlugin>();
    }
  }

  //----------------------------------------------------------------------------
  QSharedPointer<ctkPlugin> install(const QString& symbolicName, ctkPluginContext* context)
  {
    QString pluginPath = ctkPluginFrameworkLauncher::getPluginPath(symbolicName);
    if (pluginPath.isEmpty()) return QSharedPointer<ctkPlugin>();

    ctkPluginContext* pc = context;

    if (pc == 0 && fwFactory == 0)
    {
      fwFactory.reset(new ctkPluginFrameworkFactory(fwProps));
      try
      {
        fwFactory->getFramework()->init();
        pc = fwFactory->getFramework()->getPluginContext();
      }
      catch (const ctkPluginException& exc)
      {
        qCritical() << "Failed to initialize the plug-in framework:" << exc;
        fwFactory.reset();
        return QSharedPointer<ctkPlugin>();
      }
    }

    return install(QUrl::fromLocalFile(pluginPath), pc);
  }

  //----------------------------------------------------------------------------
  void resolvePlugin(const QSharedPointer<ctkPlugin>& plugin)
  {
    if (plugin)
    {
      plugin->d_func()->getUpdatedState();
    }
  }

  /*
   * Ensure all basic plugins are installed, resolved and scheduled to start. Returns a list containing
   * all basic bundles that are marked to start.
   */
  //----------------------------------------------------------------------------
  void loadBasicPlugins()
  {
    QVariant pluginsProp = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_PLUGINS);

    QVariant startOptionsProp = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_PLUGINS_START_OPTIONS);
    ctkPlugin::StartOptions startOptions = ctkPlugin::START_ACTIVATION_POLICY;
    if (startOptionsProp.isValid())
    {
      bool okay = false;
      int startOptionsInt = startOptionsProp.toInt(&okay);
      if (okay)
      {
        startOptions = ctkPlugin::StartOptions(startOptionsInt);
      }
    }

    QStringList installEntries;
    if (pluginsProp.type() == QVariant::StringList)
    {
      installEntries = pluginsProp.toStringList();
    }
    else
    {
      installEntries = pluginsProp.toString().split(',');
    }

    QList<QSharedPointer<ctkPlugin> > startEntries;
    ctkPluginContext* context = fwFactory->getFramework()->getPluginContext();
    foreach(const QString& installEntry, installEntries)
    {
      QUrl pluginUrl(installEntry);
      if (pluginUrl.isValid() && pluginUrl.scheme().isEmpty())
      {
        // try a local file path
        QFileInfo installFileInfo(installEntry);
        if (installFileInfo.exists())
        {
          pluginUrl = QUrl::fromLocalFile(installFileInfo.absoluteFilePath());
        }
        else
        {
          pluginUrl.clear();
        }
      }

      if (pluginUrl.isValid())
      {
        QSharedPointer<ctkPlugin> plugin = install(pluginUrl, context);
        if (plugin)
        {
          startEntries.push_back(plugin);
        }
      }
      else
      {
        QSharedPointer<ctkPlugin> plugin = install(installEntry, context);
        if (plugin)
        {
          // schedule all basic bundles to be started
          startEntries.push_back(plugin);
        }
      }
    }

    foreach(QSharedPointer<ctkPlugin> plugin, startEntries)
    {
      this->resolvePlugin(plugin);
    }

    foreach(QSharedPointer<ctkPlugin> plugin, startEntries)
    {
      plugin->start(startOptions);
    }
  }


  QStringList pluginSearchPaths;
  QStringList pluginLibFilter;

  ctkProperties fwProps;

  QScopedPointer<ctkPluginFrameworkFactory> fwFactory;

  bool running;

  QScopedPointer<ctkDefaultApplicationLauncher> appLauncher;
  ctkServiceRegistration appLauncherRegistration;

  QProcessEnvironment processEnv;
};

const QScopedPointer<ctkPluginFrameworkLauncherPrivate> ctkPluginFrameworkLauncher::d(
  new ctkPluginFrameworkLauncherPrivate());

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::setFrameworkProperties(const ctkProperties& props)
{
  ctkPluginFrameworkProperties::setProperties(props);
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkLauncher::run(QRunnable* endSplashHandler, const QVariant& argument)
{
  if (d->running)
  {
    throw ctkIllegalStateException("Framework already running");
  }
  {
    struct Finalize {
      ~Finalize()
      {
        try
        {
          // The application typically sets the exit code however the framework can request that
          // it be re-started. We need to check for this and potentially override the exit code.
          if (d->isForcedRestart())
          {
            ctkPluginFrameworkProperties::setProperty(PROP_EXITCODE, "23");
          }
          if (!ctkPluginFrameworkProperties::getProperty(PROP_NOSHUTDOWN).toBool())
          {
            shutdown();
          }
        }
        catch (const std::exception& e)
        {
          qWarning() << "Shutdown error:" << e.what();
        }
      }
    };
    Finalize finalizer;
    Q_UNUSED(finalizer)
    try
    {
      startup(endSplashHandler);
      if (ctkPluginFrameworkProperties::getProperty(PROP_IGNOREAPP).toBool() || d->isForcedRestart())
      {
        return argument;
      }
      return run(argument);
    }
    catch (const std::exception& e)
    {
      // ensure the splash screen is down
      if (endSplashHandler != NULL)
      {
        endSplashHandler->run();
      }
      // may use startupFailed to understand where the error happened
      if (const ctkException* ce = dynamic_cast<const ctkException*>(&e))
      {
        qWarning() << "Startup error:" << ce->printStackTrace();
      }
      else
      {
        qWarning() << "Startup error:" << e.what();
      }
    }
  }

  // we only get here if an error happened
  if (ctkPluginFrameworkProperties::getProperty(PROP_EXITCODE).isNull())
  {
    ctkPluginFrameworkProperties::setProperty(PROP_EXITCODE, "13");
    ctkPluginFrameworkProperties::setProperty(PROP_EXITDATA, QString("An error has occured. See the console output and log file for details."));
  }
  return QVariant();
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkLauncher::run(const QVariant& argument)
{
  if (!d->running)
  {
    throw ctkIllegalStateException("Framework not running.");
  }
  // if we are just initializing, do not run the application just return.
  /*
  if (d->initialize)
  {
    return 0;
  }
  */
  try
  {
    if (!d->appLauncher)
    {
      bool launchDefault = ctkPluginFrameworkProperties::getProperty(PROP_APPLICATION_LAUNCHDEFAULT, true).toBool();
      // create the ApplicationLauncher and register it as a service
      d->appLauncher.reset(new ctkDefaultApplicationLauncher(d->fwFactory->getFramework()->getPluginContext(),
                                                             ctkPluginFrameworkProperties::getProperty(PROP_ALLOW_APPRELAUNCH).toBool(),
                                                             launchDefault));
      d->appLauncherRegistration = d->fwFactory->getFramework()->getPluginContext()->
                                   registerService<ctkApplicationLauncher>(d->appLauncher.data());

      // must start the launcher AFTER service registration because this method
      // blocks and runs the application on the current thread.  This method
      // will return only after the application has stopped.
      return d->appLauncher->start(argument);
    }
    return d->appLauncher->reStart(argument);
  }
  catch (const ctkException& e)
  {
    qWarning() << "Application launch failed:" << e.printStackTrace();
    throw;
  }
  catch (const std::exception& e)
  {
    qWarning() << "Application launch failed:" << e.what();
    throw;
  }
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkPluginFrameworkLauncher::startup(QRunnable* endSplashHandler)
{
  if (d->running)
  {
    throw ctkIllegalStateException("Framework already running.");
  }
  ctkPluginFrameworkProperties::initializeProperties();
  //processCommandLine(args);
  ctkLocationManager::initializeLocations();
  d->loadConfigurationInfo();
  //finalizeProperties();
  d->fwFactory.reset(new ctkPluginFrameworkFactory(ctkPluginFrameworkProperties::getProperties()));
  //d->context = framework.getBundle(0).getBundleContext();
  //registerFrameworkShutdownHandlers();
  //publishSplashScreen(endSplashHandler);
  //consoleMgr = ConsoleManager.startConsole(framework);
  d->fwFactory->getFramework()->start();
  d->loadBasicPlugins();

  d->running = true;
  
  if (endSplashHandler != NULL)
  {
    endSplashHandler->run();
  }
  
  return d->fwFactory->getFramework()->getPluginContext();
}

void ctkPluginFrameworkLauncher::shutdown()
{
  if (!d->running || d->fwFactory == NULL)
    return;

  //if (appLauncherRegistration != null)
  //  appLauncherRegistration.unregister();
  //if (splashStreamRegistration != null)
  //  splashStreamRegistration.unregister();
  //if (defaultMonitorRegistration != null)
  //  defaultMonitorRegistration.unregister();
  if (d->appLauncher)
  {
    d->appLauncher->shutdown();
  }
  //appLauncherRegistration = null;
  //appLauncher = null;
  //splashStreamRegistration = null;
  //defaultMonitorRegistration = null;
  //d->fwFactory.reset();
  stop();
  d->running = false;
}

//----------------------------------------------------------------------------
long ctkPluginFrameworkLauncher::install(const QString& symbolicName, ctkPluginContext* context)
{
  QSharedPointer<ctkPlugin> plugin = d->install(symbolicName, context);
  if (plugin) return plugin->getPluginId();
  return -1;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkLauncher::start(const QString& symbolicName, ctkPlugin::StartOptions options,
                                       ctkPluginContext* context)
{
  // instantiate and start the framework
  if (context == 0 && d->fwFactory == 0) {
    d->fwFactory.reset(new ctkPluginFrameworkFactory(d->fwProps));
    try
    {
      d->fwFactory->getFramework()->start();
    }
    catch (const ctkPluginException& exc)
    {
      qCritical() << "Failed to start the plug-in framework:" << exc;
      d->fwFactory.reset();
      return false;
    }
  }
  else if (context == 0 && d->fwFactory->getFramework()->getState() != ctkPlugin::ACTIVE)
  {
    try
    {
      d->fwFactory->getFramework()->start(options);
    }
    catch (const ctkPluginException& exc)
    {
      qCritical() << "Failed to start the plug-in framework:" << exc;
      d->fwFactory.reset();
      return false;
    }
  }

  if(!symbolicName.isEmpty())
  {
    QString pluginPath = getPluginPath(symbolicName);
    if (pluginPath.isEmpty()) return false;

    ctkPluginContext* pc = context ? context : getPluginContext();
    try
    {
      pc->installPlugin(QUrl::fromLocalFile(pluginPath))->start(options);
    }
    catch (const ctkPluginException& exc)
    {
      qWarning() << "Failed to install plugin:" << exc;
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkLauncher::stop(const QString& symbolicName,
                                      ctkPlugin::StopOptions options, ctkPluginContext* context)
{
  if (d->fwFactory == 0) return true;

  ctkPluginContext* pc = context ? context : getPluginContext();
  if (pc == 0)
  {
    qWarning() << "No valid plug-in context available";
    return false;
  }

  if(!symbolicName.isEmpty())
  {
    QString pluginPath = getPluginPath(symbolicName);
    if (pluginPath.isEmpty()) return false;

    try
    {
      QList<QSharedPointer<ctkPlugin> > plugins = pc->getPlugins();
      foreach(QSharedPointer<ctkPlugin> plugin, plugins)
      {
        if (plugin->getSymbolicName() == symbolicName)
        {
          plugin->stop(options);
          return true;
        }
      }
      qWarning() << "Plug-in" << symbolicName << "not found";
      return false;
    }
    catch (const ctkPluginException& exc)
    {
      qWarning() << "Failed to stop plug-in:" << exc;
      return false;
    }
  }
  else
  {
    // stop the framework
    QSharedPointer<ctkPluginFramework> fw =
        qSharedPointerCast<ctkPluginFramework>(pc->getPlugin(0));
    try
    {
      fw->stop();
      ctkPluginFrameworkEvent fe = fw->waitForStop(5000);
      if (fe.getType() == ctkPluginFrameworkEvent::FRAMEWORK_WAIT_TIMEDOUT)
      {
        qWarning() << "Stopping the plugin framework timed out";
        return false;
      }
    }
    catch (const ctkRuntimeException& e)
    {
      qWarning() << "Stopping the plugin framework failed: " << e;
      return false;
    }

    return true;
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::resolve(const QSharedPointer<ctkPlugin>& plugin)
{
  d->resolvePlugin(plugin);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::resolve()
{
  QList<QSharedPointer<ctkPlugin> > plugins = getPluginFramework()->getPluginContext()->getPlugins();
  foreach(const QSharedPointer<ctkPlugin>& plugin, plugins)
  {
    resolve(plugin);
  }
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkPluginFrameworkLauncher::getPluginContext()
{
  if (d->fwFactory == 0) return 0;
  return d->fwFactory->getFramework()->getPluginContext();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPluginFramework> ctkPluginFrameworkLauncher::getPluginFramework()
{
  if (d->fwFactory)
    return d->fwFactory->getFramework();
  return QSharedPointer<ctkPluginFramework>();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::appendPathEnv(const QString& path)
{
#ifdef _WIN32
#ifdef __MINGW32__
  QString pathVar("PATH");
  QString oldPath(getenv("PATH"));
  pathVar += "=" + oldPath + ";" + path;
  if(_putenv(qPrintable(pathVar)))
#else
  std::size_t bufferLength;
  getenv_s(&bufferLength, NULL, 0, "PATH");
  QString newPath = path;
  if (bufferLength > 0)
  {
    char* oldPath = new char[bufferLength];
    getenv_s(&bufferLength, oldPath, bufferLength, "PATH");
    newPath.append(";").append(oldPath);
    delete[] oldPath;
  }
  qDebug() << "new PATH:" << newPath;
  if(_putenv_s("PATH", qPrintable(newPath)))
#endif
  {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &lpMsgBuf,
        0, NULL );

    // Avoid project configuration conflicts regarding wchar_t considered
    // a built-in type or not by using QString::fromUtf16 instead of
    // QString::fromWCharArray
    // sa http://qt-project.org/wiki/toStdWStringAndBuiltInWchar
    QString msg = QString("Adding '%1' to the PATH environment variable failed: %2")
      .arg(path).arg(QString::fromUtf16(reinterpret_cast<const ushort*>(lpMsgBuf)));

    qWarning() << msg;

    LocalFree(lpMsgBuf);
  }
#else
  Q_UNUSED(path)
#endif
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::addSearchPath(const QString& searchPath, bool addToPathEnv)
{
  d->pluginSearchPaths.prepend(searchPath);
  if (addToPathEnv) appendPathEnv(searchPath);
}

//----------------------------------------------------------------------------
QString ctkPluginFrameworkLauncher::getPluginPath(const QString& symbolicName)
{
  QString pluginFileName(symbolicName);
  pluginFileName.replace(".", "_");
  foreach(QString searchPath, d->pluginSearchPaths)
  {
    QDirIterator dirIter(searchPath, d->pluginLibFilter, QDir::Files);
    while(dirIter.hasNext())
    {
      dirIter.next();
      QFileInfo fileInfo = dirIter.fileInfo();
      QString fileBaseName = fileInfo.baseName();
      if (fileBaseName.startsWith("lib")) fileBaseName = fileBaseName.mid(3);

      if (fileBaseName == pluginFileName)
      {
        return fileInfo.canonicalFilePath();
      }
    }
  }

  return QString();
}

//----------------------------------------------------------------------------
QStringList ctkPluginFrameworkLauncher::getPluginSymbolicNames(const QString& searchPath)
{
  QStringList result;
  QDirIterator dirIter(searchPath, d->pluginLibFilter, QDir::Files);
  while(dirIter.hasNext())
  {
    dirIter.next();
    QFileInfo fileInfo = dirIter.fileInfo();
    QString fileBaseName = fileInfo.baseName();
    if (fileBaseName.startsWith("lib")) fileBaseName = fileBaseName.mid(3);
    result << fileBaseName.replace("_", ".");
  }

  return result;
}

