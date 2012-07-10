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

#ifndef CTKPLUGINPRIVATE_P_H
#define CTKPLUGINPRIVATE_P_H

#include "ctkPlugin.h"
#include "ctkPluginException.h"
#include "ctkRequirePlugin_p.h"

#include <QHash>
#include <QPluginLoader>
#include <QDateTime>
#include <QFileInfo>
#include <QMutex>
#include <QWaitCondition>


class ctkPluginActivator;
class ctkPluginArchive;
class ctkPluginFrameworkContext;

/**
 * \ingroup PluginFramework
 */
class ctkPluginPrivate {

protected:

  const QWeakPointer<ctkPlugin> q_ptr;

  class LockObject
  {
  public:

    LockObject() {};

    void lock();
    bool tryLock();
    bool tryLock(int timeout);
    void unlock();

    bool wait(unsigned long time = ULONG_MAX);
    void wakeAll();
    void wakeOne();

  private:

    Q_DISABLE_COPY(LockObject)

    QMutex m_Lock;
    QWaitCondition m_Condition;
  };

public:

  inline QWeakPointer<ctkPlugin> q_func() { return q_ptr; }
  inline QWeakPointer<const ctkPlugin> q_func() const { return q_ptr; }
  friend class ctkPlugin;

  struct Locker
  {
    Locker(LockObject* obj) : m_Obj(obj) { m_Obj->lock(); }
    ~Locker() { m_Obj->unlock(); }
  private:
      LockObject* m_Obj;
  };

  /**
   * Construct a new plugin based on a ctkPluginArchive.
   *
   * @param fw ctkPluginFrameworkContext for this plugin.
   * @param ba ctkPlugin archive representing the shared library and cached data
   * @param checkContext AccessConrolContext to do permission checks against.
   * @exception ctkInvalidArgumentException Faulty manifest for bundle
   */
  ctkPluginPrivate(QWeakPointer<ctkPlugin> qq, ctkPluginFrameworkContext* fw,
                   QSharedPointer<ctkPluginArchive> pa /*, Object checkContext*/);

  /**
   * Construct a new empty ctkPlugin.
   *
   * Only called for the system plugin
   *
   * @param fw Framework for this plugin.
   */
  ctkPluginPrivate(QWeakPointer<ctkPlugin> qq,
                   ctkPluginFrameworkContext* fw,
                   long id,
                   const QString& loc,
                   const QString& sym,
                   const ctkVersion& ver);

  virtual ~ctkPluginPrivate();

  /**
   * Get updated plugin state. That means check if an installed
   * plugin has been resolved.
   *
   * @return ctkPlugin state
   */
  ctkPlugin::State getUpdatedState();

  /**
   * Get root for persistent storage area for this plugin.
   *
   * @return A QDir object representing the data root.
   */
  QFileInfo getDataRoot();

  /**
   * Set state to INSTALLED. We assume that the plug-in is resolved
   * when entering this method.
   */
  void setStateInstalled(bool sendEvent);

  /**
   * Purge any old files associated with this plug-in.
   */
  void purge();

  /**
   * Save the autostart setting to the persistent plugin storage.
   *
   * @param setting The autostart options to save.
   */
  void setAutostartSetting(const ctkPlugin::StartOptions& setting);

  void ignoreAutostartSetting();

  void modified();

  /**
   * Performs the actual activation.
   */
  void finalizeActivation();

  const ctkRuntimeException* stop0();

  /**
   * Stop code that is executed in the pluginThread without holding the
   * operationLock.
   */
  const ctkRuntimeException* stop1();

  /**
   *
   */
  void update0(const QUrl &updateLocation, bool wasActive);

  /**
   *
   */
  int getStartLevel();

  /**
   * Wait for an ongoing operation to finish.
   *
   * @param lock QMutex used for locking.
   * @param src Caller to include in exception message.
   * @param longWait True, if we should wait extra long before aborting.
   * @throws ctkPluginException if the ongoing (de-)activation does not finish
   *         within reasonable time.
   */
  void waitOnOperation(LockObject* lock, const QString& src, bool longWait);

  /**
   *
   */
  QStringList findResourceEntries(const QString& path,
                                  const QString& pattern, bool recurse) const;

  /**
   * Union of flags allowing plugin class access
   */
  static const ctkPlugin::States RESOLVED_FLAGS;

  ctkPluginFrameworkContext * const fwCtx;

  /**
   * ctkPlugin identifier
   */
  const long id;

  /**
   * ctkPlugin location identifier
   */
  const QString location;

  /**
   * ctkPlugin symbolic name
   */
  QString symbolicName;

  /**
   * ctkPlugin version
   */
  ctkVersion version;

  /**
   * State of the plugin
   */
  ctkPlugin::State state;

  /**
   * ctkPlugin archive
   */
  QSharedPointer<ctkPluginArchive> archive;

  /**
   * Directory for plugin data
   */
  QFileInfo pluginDir;

  /**
   * ctkPluginContext for the plugin
   */
  QScopedPointer<ctkPluginContext> pluginContext;

  /**
   * ctkPluginActivator for the plugin
   */
  ctkPluginActivator* pluginActivator;

  /**
   * The Qt plugin loader for the plugin
   */
  QPluginLoader pluginLoader;

  /**
   * Time when the plugin was last modified
   */
  QDateTime lastModified;

  /**
   * Stores the default locale entries when uninstalled
   */
  QHash<QString, QString> cachedHeaders;

  /**
   * Stores the raw manifest headers
   */
  QHash<QString, QString> cachedRawHeaders;

  /**
   * Type of operation in progress. Blocks bundle calls during activator and
   * listener calls
   */
  QAtomicInt operation;
  static const int IDLE = 0;
  static const int ACTIVATING = 1;
  static const int DEACTIVATING = 2;
  static const int RESOLVING = 3;
  static const int UNINSTALLING = 4;
  static const int UNRESOLVING = 5;
  static const int UPDATING = 6;

  LockObject operationLock;

  /** Saved exception of resolve failure. */
  ctkPluginException* resolveFailException;

  /**
   * True when this plugin has its activation policy
   * set to "eager"
   */
  bool eagerActivation;

  /** List of ctkRequirePlugin entries. */
  QList<ctkRequirePlugin*> require;

private:

  /** Rember if plugin was started */
  bool wasStarted;

  /**
   * Check manifest and cache certain manifest headers as variables.
   */
  void checkManifestHeaders();

  // This could potentially be run in its own thread,
  // parallelizing plugin activations
  ctkPluginException* start0();

  void startDependencies();

  /**
   * Remove a plugins registered listeners, registered services and
   * used services.
   *
   */
  void removePluginResources();

  ctkPlugin::State getUpdatedState_unlocked();

};


#endif // CTKPLUGINPRIVATE_P_H
