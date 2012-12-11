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

#include "ctkPlugin_p.h"
#include "ctkPluginConstants.h"
#include "ctkPluginDatabaseException.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginActivator.h"
#include "ctkPluginContext_p.h"

#include "ctkServiceReference_p.h"
#include "ctkServiceRegistration.h"
#include "ctkServices_p.h"

// for ctk::msecsTo() - remove after switching to Qt 4.7
#include <ctkUtils.h>

#include <typeinfo>

const ctkPlugin::States ctkPluginPrivate::RESOLVED_FLAGS = ctkPlugin::RESOLVED | ctkPlugin::STARTING | ctkPlugin::ACTIVE | ctkPlugin::STOPPING;

//----------------------------------------------------------------------------
void ctkPluginPrivate::LockObject::lock()
{
  m_Lock.lock();
}

//----------------------------------------------------------------------------
bool ctkPluginPrivate::LockObject::tryLock()
{
  return m_Lock.tryLock();
}

//----------------------------------------------------------------------------
bool ctkPluginPrivate::LockObject::tryLock(int timeout)
{
  return m_Lock.tryLock(timeout);
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::LockObject::unlock()
{
  m_Lock.unlock();
}

//----------------------------------------------------------------------------
bool ctkPluginPrivate::LockObject::wait(unsigned long time)
{
  return m_Condition.wait(&m_Lock, time);
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::LockObject::wakeAll()
{
  m_Condition.wakeAll();
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::LockObject::wakeOne()
{
  m_Condition.wakeOne();
}

//----------------------------------------------------------------------------
ctkPluginPrivate::ctkPluginPrivate(
    QWeakPointer<ctkPlugin> qq,
    ctkPluginFrameworkContext* fw,
    QSharedPointer<ctkPluginArchive> pa)
      : q_ptr(qq), fwCtx(fw), id(pa->getPluginId()),
      location(pa->getPluginLocation().toString()), state(ctkPlugin::INSTALLED),
      archive(pa), pluginContext(0), pluginActivator(0), pluginLoader(pa->getLibLocation()),
      resolveFailException(0), eagerActivation(false), wasStarted(false)
{
  //TODO
  //checkCertificates(pa);

  // Get library load hints
  if (fw->props.contains(ctkPluginConstants::FRAMEWORK_PLUGIN_LOAD_HINTS))
  {
    QVariant loadHintsVariant = fw->props[ctkPluginConstants::FRAMEWORK_PLUGIN_LOAD_HINTS];
    if (loadHintsVariant.isValid())
    {
      QLibrary::LoadHints loadHints = loadHintsVariant.value<QLibrary::LoadHints>();
      pluginLoader.setLoadHints(loadHints);
    }
  }

  checkManifestHeaders();

  pluginDir = fwCtx->getDataStorage(id);
//  int oldStartLevel = archive->getStartLevel();
  try
  {
    //TODO: StartLevel Service
    //if (fwCtx->startLevelController == 0)
    //{
      archive->setStartLevel(0);
    //}
//    else
//    {
//      if (oldStartLevel == -1)
//      {
//        archive->setStartLevel(fwCtx->startLevelController->getInitialPluginStartLevel());
//      }
//    }
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to set start level on #" << id << ":" << e.what();
  }

  lastModified = archive->getLastModified();
  if (lastModified.isNull())
  {
    modified();
  }

  // fill require list
  QString requireString = archive->getAttribute(ctkPluginConstants::REQUIRE_PLUGIN);
  QList<QMap<QString, QStringList> > requireList = ctkPluginFrameworkUtil::parseEntries(ctkPluginConstants::REQUIRE_PLUGIN,
                                                                                        requireString, true, true, false);
  QListIterator<QMap<QString, QStringList> > i(requireList);
  while (i.hasNext())
  {
    const QMap<QString, QStringList>& e = i.next();
    const QStringList& res = e.value(ctkPluginConstants::RESOLUTION_DIRECTIVE);
    const QStringList& version = e.value(ctkPluginConstants::PLUGIN_VERSION_ATTRIBUTE);
    ctkRequirePlugin* rp = new ctkRequirePlugin(this, e.value("$key").front(),
                                                res.empty() ? QString() : res.front(),
                                                version.empty() ? QString() : version.front());
    require.push_back(rp);
  }
}

//----------------------------------------------------------------------------
ctkPluginPrivate::ctkPluginPrivate(QWeakPointer<ctkPlugin> qq,
                                   ctkPluginFrameworkContext* fw,
                                   long id, const QString& loc, const QString& sym, const ctkVersion& ver)
                                     : q_ptr(qq), fwCtx(fw), id(id), location(loc), symbolicName(sym), version(ver),
                                       state(ctkPlugin::INSTALLED), archive(0), pluginContext(0),
                                       pluginActivator(0), resolveFailException(0),
                                       eagerActivation(false), wasStarted(false)
{
  modified();
}

//----------------------------------------------------------------------------
ctkPluginPrivate::~ctkPluginPrivate()
{
  qDeleteAll(require);
}

//----------------------------------------------------------------------------
ctkPlugin::State ctkPluginPrivate::getUpdatedState()
{
  if (state & ctkPlugin::INSTALLED)
  {
    Locker sync(&operationLock);
    getUpdatedState_unlocked();
  }
  return state;
}

//----------------------------------------------------------------------------
ctkPlugin::State ctkPluginPrivate::getUpdatedState_unlocked()
{
  if (state & ctkPlugin::INSTALLED)
  {
    try
    {
      if (state == ctkPlugin::INSTALLED)
      {
        operation.fetchAndStoreOrdered(RESOLVING);
        fwCtx->resolvePlugin(this);
        state = ctkPlugin::RESOLVED;
        // TODO plugin threading
        //bundleThread().bundleChanged(new BundleEvent(BundleEvent.RESOLVED, this));
        fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::RESOLVED, this->q_func()));
        operation.fetchAndStoreOrdered(IDLE);
      }
    }
    catch (const ctkPluginException& pe)
    {
      if (resolveFailException) delete resolveFailException;
      resolveFailException = new ctkPluginException(pe);
      this->fwCtx->listeners.frameworkError(this->q_func(), pe);
    }
  }
  return state;
}

//----------------------------------------------------------------------------
QFileInfo ctkPluginPrivate::getDataRoot()
{
  return pluginDir;
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::setStateInstalled(bool sendEvent)
{
  Locker sync(&operationLock);

  // Make sure that the context is invalid
  if (pluginContext != 0)
  {
    pluginContext->d_func()->invalidate();
    pluginContext.reset();
  }
  state = ctkPlugin::INSTALLED;
  if (sendEvent)
  {
    operation.fetchAndStoreOrdered(UNRESOLVING);
    // TODO: plugin thread
    //bundleThread().bundleChanged(new BundleEvent(BundleEvent.UNRESOLVED, this));
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::UNRESOLVED, this->q_func()));
  }
  operation.fetchAndStoreOrdered(IDLE);
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::purge()
{
  if (state == ctkPlugin::UNINSTALLED)
  {
    fwCtx->plugins->remove(location);
  }
//  Vector fix = oldGenerations;
//  if (fix != null) {
//    oldGenerations = null;
//    for (Iterator i = fix.iterator(); i.hasNext();) {
//      ((BundleGeneration)i.next()).purge(true);
//    }
//  }
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::setAutostartSetting(const ctkPlugin::StartOptions& setting) {
  try
  {
    if (archive)
    {
      archive->setAutostartSetting(setting);
    }
  }
  catch (const ctkPluginDatabaseException& e)
  {
    this->fwCtx->listeners.frameworkError(this->q_func(), e);
  }
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::ignoreAutostartSetting()
{
  try
  {
    if (archive)
    {
      archive->setAutostartSetting(-1);
    }
  }
  catch (const ctkPluginDatabaseException& e)
  {
    this->fwCtx->listeners.frameworkError(this->q_func(), e);
  }
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::modified()
{
  lastModified = QDateTime::currentDateTime();
  if (archive)
  {
    archive->setLastModified(lastModified);
  }
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::checkManifestHeaders()
{
  symbolicName = archive->getAttribute(ctkPluginConstants::PLUGIN_SYMBOLICNAME);

  if (symbolicName.isEmpty())
  {
    throw ctkInvalidArgumentException(QString("ctkPlugin has no symbolic name, location=") +
                                      location);
  }

  QString mpv = archive->getAttribute(ctkPluginConstants::PLUGIN_VERSION);
  if (!mpv.isEmpty())
  {
    try
    {
      version = ctkVersion(mpv);
    }
    catch (const std::exception& e)
    {
      throw ctkInvalidArgumentException(QString("ctkPlugin does not specify a valid ") +
                                        ctkPluginConstants::PLUGIN_VERSION + " header. Got exception: " + e.what());
    }
  }

  QSharedPointer<ctkPlugin> snp = fwCtx->plugins->getPlugin(symbolicName, version);
  // TBD! Should we allow update to same version?
  if (!snp.isNull() && snp->d_func() != this)
  {
    throw ctkInvalidArgumentException(QString("Plugin with same symbolic name and version is already installed (")
                                      + symbolicName + ", " + version.toString() + ")");
  }

  QString ap = archive->getAttribute(ctkPluginConstants::PLUGIN_ACTIVATIONPOLICY);
  if (ctkPluginConstants::ACTIVATION_EAGER == ap)
  {
    eagerActivation = true;
  }

}

//----------------------------------------------------------------------------
void ctkPluginPrivate::finalizeActivation()
{
  Locker sync(&operationLock);

  // 4: Resolve plugin (if needed)
  switch (getUpdatedState_unlocked())
  {
  case ctkPlugin::INSTALLED:
    Q_ASSERT_X(resolveFailException != 0, Q_FUNC_INFO, "no resolveFailException");
    throw ctkPluginException(*resolveFailException);
  case ctkPlugin::STARTING:
    if (operation.fetchAndAddOrdered(0) == ACTIVATING) return; // finalization already in progress.
    // Lazy activation; fall through to RESOLVED.
  case ctkPlugin::RESOLVED:
  {
    //6:
    state = ctkPlugin::STARTING;
    operation.fetchAndStoreOrdered(ACTIVATING);
    if (fwCtx->debug.lazy_activation)
    {
      qDebug() << "activating #" << this->id;
    }
    //7:
    if (!pluginContext)
    {
      pluginContext.reset(new ctkPluginContext(this));
    }
    // start dependencies
    startDependencies();
    //TODO plugin threading
    //ctkRuntimeException* e = bundleThread().callStart0(this);
    ctkRuntimeException* e = start0();
    operation.fetchAndStoreOrdered(IDLE);
    operationLock.wakeAll();
    if (e)
    {
      ctkRuntimeException re(*e);
      delete e;
      throw re;
    }
    break;
  }
  case ctkPlugin::ACTIVE:
    break;
  case ctkPlugin::STOPPING:
    // This happens if start is called from inside the ctkPluginActivator::stop method.
    // Don't allow it.
    throw ctkPluginException("start called from ctkPluginActivator::stop",
                             ctkPluginException::ACTIVATOR_ERROR);
  case ctkPlugin::UNINSTALLED:
    throw ctkIllegalStateException("ctkPlugin is in UNINSTALLED state");
  }
}

//----------------------------------------------------------------------------
const ctkRuntimeException* ctkPluginPrivate::stop0()
{
  wasStarted = state == ctkPlugin::ACTIVE;
  // 5:
  state = ctkPlugin::STOPPING;
  operation.fetchAndStoreOrdered(DEACTIVATING);
  // 6-13:
  // TODO plugin threading
  //const ctkRuntimeException* savedException = pluginThread().callStop1(this);
  const ctkRuntimeException* savedException = stop1();
  if (state != ctkPlugin::UNINSTALLED)
  {
    state = ctkPlugin::RESOLVED;
    // TODO plugin threading
    //bundleThread().bundleChanged(new BundleEvent(BundleEvent.STOPPED, this));
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPED, this->q_func()));

    operationLock.wakeAll();
    operation.fetchAndStoreOrdered(IDLE);
  }
  return savedException;
}

//----------------------------------------------------------------------------
const ctkRuntimeException* ctkPluginPrivate::stop1()
{
  const ctkRuntimeException* res = 0;

  //6:
  fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPING, q_func()));

  //7:
  if (wasStarted && pluginActivator)
  {
    try
    {
      pluginActivator->stop(pluginContext.data());
      if (state != ctkPlugin::STOPPING)
      {
        if (state == ctkPlugin::UNINSTALLED)
        {
          return new ctkIllegalStateException("Plug-in is uninstalled");
        }
        else
        {
          return new ctkIllegalStateException("Plug-in changed state because of refresh during stop");
        }
      }
    }
    catch (const ctkException& e)
    {
      res = new ctkPluginException("ctkPlugin::stop: PluginActivator stop failed",
                                   ctkPluginException::ACTIVATOR_ERROR, e);
    }
    catch (...)
    {
      res = new ctkPluginException("ctkPlugin::stop: PluginActivator stop failed",
                                   ctkPluginException::ACTIVATOR_ERROR);
    }
    pluginActivator = 0;
  }

  if (operation.fetchAndAddOrdered(0) == DEACTIVATING)
  {
    // Call hooks after we've called PluginActivator::stop(), but before we've
    // cleared all resources
    if (pluginContext)
    {
      // TODO service listener hooks
      //fwCtx->listeners.serviceListeners.hooksBundleStopped(this);

      //8-10:
      removePluginResources();
      pluginContext->d_func()->invalidate();
      pluginContext.reset();
    }
  }

  // This would unload the shared library and delete the activator if
  // there are no dependencies. However, objects from the plug-in might
  // have been created via C-function symbol lookups. Hence we cannot
  // safely unload the DLL. Maybe implement a in-DLL counter later
  // (http://stackoverflow.com/questions/460809/c-dll-unloading-issue and
  // http://boost.2283326.n4.nabble.com/shared-ptr-A-smarter-smart-pointer-proposal-for-dynamic-libraries-td2649749.html).
  // The activator itself will be delete during program termination
  // (by the QPluginLoader instance).
  //pluginLoader.unload();

  return res;
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::update0(const QUrl& updateLocation, bool wasActive)
{
  const bool wasResolved = state == ctkPlugin::RESOLVED;
  const int oldStartLevel = getStartLevel();
  QSharedPointer<ctkPluginArchive> newArchive;

  operation.fetchAndStoreOrdered(UPDATING);
  try
  {
    // New plugin as stream supplied?
    QUrl updateUrl(updateLocation);
    if (updateUrl.isEmpty())
    {
      // Try Plugin-UpdateLocation
      QString update = archive != 0 ? archive->getAttribute(ctkPluginConstants::PLUGIN_UPDATELOCATION) : QString();
      if (update.isEmpty())
      {
        // Take original location
        updateUrl = location;
      }
    }

    if(updateUrl.scheme() != "file")
    {
      QString msg = "Unsupported update URL:";
      msg += updateUrl.toString();
      throw ctkPluginException(msg);
    }

    newArchive = fwCtx->storage->updatePluginArchive(archive, updateUrl, updateUrl.toLocalFile());
    //checkCertificates(newArchive);
    checkManifestHeaders();
    newArchive->setStartLevel(oldStartLevel);
    fwCtx->storage->replacePluginArchive(archive, newArchive);
  }
  catch (const std::exception& e)
  {
    if (!newArchive.isNull())
    {
      newArchive->purge();
    }
    operation.fetchAndStoreOrdered(IDLE);
    if (wasActive)
    {
      try
      {
        this->q_func().data()->start();
      }
      catch (const ctkPluginException& pe)
      {
        fwCtx->listeners.frameworkError(this->q_func(), pe);
      }
    }
    try
    {
      const ctkPluginException& pe = dynamic_cast<const ctkPluginException&>(e);
      throw pe;
    }
    catch (std::bad_cast)
    {
      throw ctkPluginException(QString("Failed to get update plugin: ") + e.what(),
                               ctkPluginException::UNSPECIFIED);
    }
  }

  bool purgeOld = false;
  // TODO: check if dependent plug-ins are started. If not, set purgeOld to true.

  // Activate new plug-in
  QSharedPointer<ctkPluginArchive> oldArchive = archive;
  archive = newArchive;
  cachedRawHeaders.clear();
  state = ctkPlugin::INSTALLED;

  // Purge old archive
  if (purgeOld)
  {
    //secure.purge(this, oldProtectionDomain);
    if (oldArchive != 0)
    {
      oldArchive->purge();
    }
  }

  // Broadcast events
  if (wasResolved)
  {
    // TODO: use plugin threading
    //bundleThread().bundleChanged(new BundleEvent(BundleEvent.UNRESOLVED, this));
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::UNRESOLVED, this->q_func()));
  }
  //bundleThread().bundleChanged(new BundleEvent(BundleEvent.UPDATED, this));
  fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::UPDATED, this->q_func()));
  operation.fetchAndStoreOrdered(IDLE);

   // Restart plugin previously stopped in the operation
   if (wasActive)
   {
     try
     {
       this->q_func().data()->start();
     }
     catch (const ctkPluginException& pe)
     {
       fwCtx->listeners.frameworkError(this->q_func(), pe);
     }
   }
 }

//----------------------------------------------------------------------------
int ctkPluginPrivate::getStartLevel()
{
  if (archive != 0)
  {
    return archive->getStartLevel();
  }
  else
  {
    return 0;
  }
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::waitOnOperation(LockObject* lock, const QString& src, bool longWait)
{
  if (operation.fetchAndAddOrdered(0) != IDLE)
  {
    qint64 left = longWait ? 20000 : 500;
    QDateTime waitUntil = QDateTime::currentDateTime().addMSecs(left);
    do
    {
      lock->wait(left);
      if (operation.fetchAndAddOrdered(0) == IDLE)
      {
        return;
      }
      // TODO use Qt 4.7 QDateTime::msecsTo() API
      //left = QDateTime::currentDateTime().msecsTo(waitUntil);
      left = ctk::msecsTo(QDateTime::currentDateTime(), waitUntil);
    } while (left > 0);

    QString op;
    switch (operation.fetchAndAddOrdered(0))
    {
    case IDLE:
      // Should not happen!
      return;
    case ACTIVATING:
      op = "start";
      break;
    case DEACTIVATING:
      op = "stop";
      break;
    case RESOLVING:
      op = "resolve";
      break;
    case UNINSTALLING:
      op = "uninstall";
      break;
    case UNRESOLVING:
      op = "unresolve";
      break;
    case UPDATING:
      op = "update";
      break;
    default:
      op = "unknown operation";
      break;
    }
    throw ctkPluginException(src + " called during " + op + " of plug-in",
                             ctkPluginException::STATECHANGE_ERROR);
  }
}

//----------------------------------------------------------------------------
QStringList ctkPluginPrivate::findResourceEntries(const QString& path,
                                                  const QString& pattern, bool recurse) const
{
  QStringList result;
  QStringList resources = archive->findResourcesPath(path);
  foreach(QString fp, resources)
  {
    QString lastComponentOfPath = fp.section('/', -1);
    bool isDirectory = fp.endsWith("/");

    if (!isDirectory &&
        (pattern.isNull() || ctkPluginFrameworkUtil::filterMatch(pattern, lastComponentOfPath)))
    {
      result << (path + fp);
    }
    if (isDirectory && recurse)
    {
      QStringList subResources = findResourceEntries(fp, pattern, recurse);
      foreach (QString subResource, subResources)
      {
        result << (path + subResource);
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::startDependencies()
{
  QListIterator<ctkRequirePlugin*> i(this->require);
  while (i.hasNext())
  {
    ctkRequirePlugin* pr = i.next();
    QList<ctkPlugin*> pl = fwCtx->plugins->getPlugins(pr->name, pr->pluginRange);
    if (pl.isEmpty())
    {
      if (pr->resolution == ctkPluginConstants::RESOLUTION_MANDATORY)
      {
        // We should never get here, since the plugin can only be
        // started if all its dependencies could be resolved.
        throw ctkPluginException(
            QString("Internal error: dependent plugin %1 inside version range %2 is not installed.").
            arg(pr->name).arg(pr->pluginRange.toString()));
      }
      else
      {
        continue;
      }
    }

    // We take the first plugin in the list (highest version number)
    // Immediately start the dependencies (no lazy activation) but do not
    // change the autostart setting of the plugin.
    pl.front()->start(ctkPlugin::START_TRANSIENT);
  }
}

//----------------------------------------------------------------------------
ctkPluginException* ctkPluginPrivate::start0()
{
  ctkPluginException* res = 0;

  fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STARTING, this->q_func()));

  ctkPluginException::Type error_type = ctkPluginException::MANIFEST_ERROR;
  try {
    pluginLoader.load();
    if (!pluginLoader.isLoaded())
    {
      error_type = ctkPluginException::ACTIVATOR_ERROR;
      throw ctkPluginException(QString("Loading plugin %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                               ctkPluginException::ACTIVATOR_ERROR);
    }

    pluginActivator = qobject_cast<ctkPluginActivator*>(pluginLoader.instance());
    if (!pluginActivator)
    {
      throw ctkPluginException(QString("Creating ctkPluginActivator instance from %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                               ctkPluginException::ACTIVATOR_ERROR);
    }

    pluginActivator->start(pluginContext.data());

    if (state != ctkPlugin::STARTING)
    {
      error_type = ctkPluginException::STATECHANGE_ERROR;
      if (ctkPlugin::UNINSTALLED == state)
      {
        throw ctkPluginException("ctkPlugin uninstalled during start()", ctkPluginException::STATECHANGE_ERROR);
      }
      else
      {
        throw ctkPluginException("ctkPlugin changed state because of refresh during start()", ctkPluginException::STATECHANGE_ERROR);
      }
    }
    state = ctkPlugin::ACTIVE;
  }
  catch (const ctkException& e)
  {
    res = new ctkPluginException("ctkPlugin start failed", error_type, e);
  }
  catch (...)
  {
    res = new ctkPluginException("ctkPlugin start failed", error_type);
  }

  if (fwCtx->debug.lazy_activation)
  {
    qDebug() << "activating #" << id << "completed.";
  }

  if (res == 0)
  {
    //10:
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STARTED, this->q_func()));
  }
  else if (operation.fetchAndAddOrdered(0) == ACTIVATING)
  {
    // 8:
    state = ctkPlugin::STOPPING;
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPING, this->q_func()));
    removePluginResources();
    pluginContext->d_func()->invalidate();
    pluginContext.reset();
    state = ctkPlugin::RESOLVED;
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPED, this->q_func()));
  }

  return res;
}

//----------------------------------------------------------------------------
void ctkPluginPrivate::removePluginResources()
{
  // automatic disconnect due to Qt signal slot
  //fwCtx->listeners.removeAllListeners(this);

  QList<ctkServiceRegistration> srs = fwCtx->services->getRegisteredByPlugin(this);
  QMutableListIterator<ctkServiceRegistration> i(srs);
  while (i.hasNext())
  {
    try
    {
      i.next().unregister();
    }
    catch (const ctkIllegalStateException& /*ignore*/)
    {
      // Someone has unregistered the service after stop completed.
      // This should not occur, but we don't want get stuck in
      // an illegal state so we catch it.
    }
  }

  QList<ctkServiceRegistration> s = fwCtx->services->getUsedByPlugin(q_func());
  QListIterator<ctkServiceRegistration> i2(s);
  while (i2.hasNext())
  {
    i2.next().getReference().d_func()->ungetService(q_func(), false);
  }

}
