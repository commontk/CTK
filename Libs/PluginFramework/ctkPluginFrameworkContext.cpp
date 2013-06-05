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

#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginFramework_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginStorageSQL_p.h"
#include "ctkPluginConstants.h"
#include "ctkServices_p.h"
#include "ctkUtils.h"

//----------------------------------------------------------------------------
QMutex ctkPluginFrameworkContext::globalFwLock;
int ctkPluginFrameworkContext::globalId = 1;

//----------------------------------------------------------------------------
ctkPluginFrameworkContext::ctkPluginFrameworkContext(
    const ctkProperties& initProps)
  : plugins(0), listeners(this), services(0), systemPlugin(new ctkPluginFramework()),
    storage(0), firstInit(true), props(initProps), debug(props),
    initialized(false)
{

  {
    QMutexLocker lock(&globalFwLock);
    id = globalId++;
    systemPlugin->ctkPlugin::init(new ctkPluginFrameworkPrivate(systemPlugin, this));
  }

  initProperties();
  log() << "created";
}

//----------------------------------------------------------------------------
ctkPluginFrameworkContext::~ctkPluginFrameworkContext()
{
  if (initialized)
  {
    this->uninit();
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::initProperties()
{
  props[ctkPluginConstants::FRAMEWORK_VERSION] = "0.9";
  props[ctkPluginConstants::FRAMEWORK_VENDOR] = "CommonTK";
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::init()
{
  log() << "initializing";

  if (firstInit && ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT
      == props[ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN])
  {
    deleteFWDir();
    firstInit = false;
  }

  // Pre-load libraries
  // This may speed up installing new plug-ins if they have dependencies on
  // one of these libraries. It prevents repeated loading and unloading of the
  // pre-loaded libraries during caching of the plug-in meta-data.
  if (props[ctkPluginConstants::FRAMEWORK_PRELOAD_LIBRARIES].isValid())
  {
    QStringList preloadLibs = props[ctkPluginConstants::FRAMEWORK_PRELOAD_LIBRARIES].toStringList();
    QLibrary::LoadHints loadHints;
    QVariant loadHintsVariant = props[ctkPluginConstants::FRAMEWORK_PLUGIN_LOAD_HINTS];
    if (loadHintsVariant.isValid())
    {
      loadHints = loadHintsVariant.value<QLibrary::LoadHints>();
    }

    foreach(QString preloadLib, preloadLibs)
    {
      QLibrary lib;
      QStringList nameAndVersion = preloadLib.split(":");

      QString libraryName;
      if (nameAndVersion.count() == 1)
      {
        libraryName = nameAndVersion.front();
        lib.setFileName(nameAndVersion.front());
      }
      else if (nameAndVersion.count() == 2)
      {
        libraryName = nameAndVersion.front() + "." + nameAndVersion.back();
        lib.setFileNameAndVersion(nameAndVersion.front(), nameAndVersion.back());
      }
      else
      {
        qWarning() << "Wrong syntax in" << preloadLib << ". Use <lib-name>[:version]. Skipping.";
        continue;
      }

      lib.setLoadHints(loadHints);
      log() << "Pre-loading library" << lib.fileName() << "with hints [" << static_cast<int>(loadHints) << "]";
      if (!lib.load())
      {
        qWarning() << "Pre-loading library" << lib.fileName() << "failed:" << lib.errorString() << "\nCheck your library search paths.";
      }
    }
  }

  ctkPluginFrameworkPrivate* const systemPluginPrivate = systemPlugin->d_func();
  systemPluginPrivate->initSystemPlugin();

  storage = new ctkPluginStorageSQL(this);
  dataStorage = ctkPluginFrameworkUtil::getFileStorage(this, "data");
  services = new ctkServices(this);
  plugins = new ctkPlugins(this);

  plugins->load();

  log() << "inited";
  initialized = true;

  log() << "Installed plugins:";
  // Use the ordering in the plugin storage to get a sorted list of plugins.
  QList<QSharedPointer<ctkPluginArchive> > allPAs = storage->getAllPluginArchives();
  foreach (QSharedPointer<ctkPluginArchive> pa, allPAs)
  {
    QSharedPointer<ctkPlugin> plugin = plugins->getPlugin(pa->getPluginLocation().toString());
    log() << " #" << plugin->getPluginId() << " " << plugin->getSymbolicName() << ":"
        << plugin->getVersion() << " location:" << plugin->getLocation();
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::uninit()
{
  if (!initialized) return;

  log() << "uninit";

  ctkPluginFrameworkPrivate* const systemPluginPrivate = systemPlugin->d_func();
  systemPluginPrivate->uninitSystemPlugin();

  plugins->clear();
  delete plugins;
  plugins = 0;

  delete storage; // calls storage->close()
  storage = 0;

  delete services;
  services = 0;

  initialized = false;
}

//----------------------------------------------------------------------------
int ctkPluginFrameworkContext::getId() const
{
  return id;
}

//----------------------------------------------------------------------------
QFileInfo ctkPluginFrameworkContext::getDataStorage(long id)
{
  return QFileInfo(dataStorage.absolutePath() + '/' + QString::number(id) + '/');
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::checkOurPlugin(ctkPlugin* plugin) const
{
  ctkPluginPrivate* pp = plugin->d_func();
  if (this != pp->fwCtx)
  {
    throw ctkInvalidArgumentException("ctkPlugin does not belong to this framework: " + plugin->getSymbolicName());
  }
}

//----------------------------------------------------------------------------
QDebug ctkPluginFrameworkContext::log() const
{
  static QString nirvana;
  nirvana.clear();
  if (debug.framework)
    return qDebug() << "Framework instance " << getId() << ": ";
  else
    return QDebug(&nirvana);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::resolvePlugin(ctkPluginPrivate* plugin)
{
  if (debug.resolve)
  {
    qDebug() << "resolve:" << plugin->symbolicName << "[" << plugin->id << "]";
  }

  // If we enter with tempResolved set, it means that we already have
  // resolved plugins. Check that it is true!
  if (tempResolved.size() > 0 && !tempResolved.contains(plugin))
  {
    ctkPluginException pe("resolve: InternalError1!", ctkPluginException::RESOLVE_ERROR);
    listeners.frameworkError(plugin->q_func(), pe);
    throw pe;
  }

  tempResolved.clear();
  tempResolved.insert(plugin);

  checkRequirePlugin(plugin);

  tempResolved.clear();

  if (debug.resolve)
  {
    qDebug() << "resolve: Done for" << plugin->symbolicName << "[" << plugin->id << "]";
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::checkRequirePlugin(ctkPluginPrivate *plugin)
{
  if (!plugin->require.isEmpty())
  {
    if (debug.resolve)
    {
      qDebug() << "checkRequirePlugin: check requiring plugin" << plugin->id;
    }

    QListIterator<ctkRequirePlugin*> i(plugin->require);
    while (i.hasNext())
    {
      ctkRequirePlugin* pr = i.next();
      QList<ctkPlugin*> pl = plugins->getPlugins(pr->name, pr->pluginRange);
      ctkPluginPrivate* ok = 0;
      for (QListIterator<ctkPlugin*> pci(pl); pci.hasNext() && ok == 0; )
      {
        ctkPluginPrivate* p2 = pci.next()->d_func();
        if (tempResolved.contains(p2))
        {
          ok = p2;
        }
        else if (ctkPluginPrivate::RESOLVED_FLAGS & p2->state)
        {
          ok = p2;
        }
        else if (p2->state == ctkPlugin::INSTALLED) {
          QSet<ctkPluginPrivate*> oldTempResolved = tempResolved;
          tempResolved.insert(p2);

          // TODO check if operation locking is correct in case of
          // multi-threaded plug-in start up. Maybe refactor out the dependency
          // checking (use the "package" lock)
          ctkPluginPrivate::Locker sync(&p2->operationLock);
          p2->operation.fetchAndStoreOrdered(ctkPluginPrivate::RESOLVING);
          checkRequirePlugin(p2);
          tempResolved = oldTempResolved;
          p2->state = ctkPlugin::RESOLVED;
          listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::RESOLVED, p2->q_func()));
          p2->operation.fetchAndStoreOrdered(ctkPluginPrivate::IDLE);
          ok = p2;
        }
      }

      if (!ok && pr->resolution == ctkPluginConstants::RESOLUTION_MANDATORY)
      {
        tempResolved.clear();
        if (debug.resolve)
        {
          qDebug() << "checkRequirePlugin: failed to satisfy:" << pr->name;
        }
        throw ctkPluginException(QString("Failed to resolve required plugin: %1").arg(pr->name));
      }
    }
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkContext::deleteFWDir()
{
  QString d = ctkPluginFrameworkUtil::getFrameworkDir(this);

  QFileInfo fwDirInfo(d);
  if (fwDirInfo.exists())
  {
    if(fwDirInfo.isDir())
    {
      log() << "deleting old framework directory.";
      bool bOK = ctk::removeDirRecursively(fwDirInfo.absoluteFilePath());
      if(!bOK)
      {
        qDebug() << "Failed to remove existing fwdir" << fwDirInfo.absoluteFilePath();
      }
    }
  }
}
