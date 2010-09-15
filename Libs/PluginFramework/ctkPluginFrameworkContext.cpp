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

#include "ctkPluginFrameworkContext_p.h"

#include "ctkPluginFrameworkPrivate_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginConstants.h"


  QMutex ctkPluginFrameworkContext::globalFwLock;
  int ctkPluginFrameworkContext::globalId = 1;


  ctkPluginFrameworkContext::ctkPluginFrameworkContext(
      const ctkPluginFrameworkFactory::Properties& initProps)
        : plugins(0), services(this), systemPlugin(this),
        storage(this), props(initProps)
  {

    {
      QMutexLocker lock(&globalFwLock);
      id = globalId++;
    }

    log() << "created";
  }

  void ctkPluginFrameworkContext::init()
  {
    log() << "initializing";

//    if (Constants.FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT
//        .equals(props.getProperty(Constants.FRAMEWORK_STORAGE_CLEAN))) {
//      deleteFWDir();
//      // Must remove the storage clean property since it should not be
//      // used more than once!
//      props.removeProperty(Constants.FRAMEWORK_STORAGE_CLEAN);
//    }
//    props.save();

    ctkPluginFrameworkPrivate* const systemPluginPrivate = systemPlugin.d_func();
    systemPluginPrivate->initSystemPlugin();

    plugins = new ctkPlugins(this);

    plugins->load();

    log() << "inited";

    log() << "Installed plugins:";
    // Use the ordering in the plugin storage to get a sorted list of plugins.
    QList<ctkPluginArchive*> allPAs = storage.getAllPluginArchives();
    for (int i = 0; i < allPAs.size(); ++i)
    {
      ctkPluginArchive* pa = allPAs[i];
      ctkPlugin* plugin = plugins->getPlugin(pa->getPluginLocation().toString());
      log() << " #" << plugin->getPluginId() << " " << plugin->getSymbolicName() << ":"
          << plugin->getVersion() << " location:" << plugin->getLocation();
    }
  }

  void ctkPluginFrameworkContext::uninit()
  {
    log() << "uninit";

    //ctkPluginFrameworkPrivate* const systemPluginPrivate = systemPlugin.d_func();
    //systemPluginPrivate->uninitSystemBundle();

    plugins->clear();
    delete plugins;
    plugins = 0;

    storage.close();

  }

  int ctkPluginFrameworkContext::getId() const
  {
    return id;
  }

  void ctkPluginFrameworkContext::checkOurPlugin(ctkPlugin* plugin) const
  {
    ctkPluginPrivate* pp = plugin->d_func();
    if (this != pp->fwCtx)
    {
      throw std::invalid_argument("ctkPlugin does not belong to this framework: " + plugin->getSymbolicName().toStdString());
    }
  }

  QDebug ctkPluginFrameworkContext::log() const
  {
    QDebug dbg(qDebug());
    dbg << "Framework instance " << getId() << ": ";
    return dbg;
  }

  void ctkPluginFrameworkContext::resolvePlugin(ctkPluginPrivate* plugin)
  {
    qDebug() << "resolve:" << plugin->symbolicName << "[" << plugin->id << "]";

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

    qDebug() << "resolve: Done for" << plugin->symbolicName << "[" << plugin->id << "]";
  }

  void ctkPluginFrameworkContext::checkRequirePlugin(ctkPluginPrivate *plugin)
  {
    if (!plugin->require.isEmpty())
    {
      qDebug() << "checkRequirePlugin: check requiring plugin" << plugin->id;

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
            checkRequirePlugin(p2);
            tempResolved = oldTempResolved;
            ok = p2;
          }
        }

        if (!ok && pr->resolution == ctkPluginConstants::RESOLUTION_MANDATORY)
        {
          tempResolved.clear();
          qDebug() << "checkRequirePlugin: failed to satisfy:" << pr->name;
          throw ctkPluginException(QString("Failed to resolve required plugin: %1").arg(pr->name));
        }
      }
    }


}
