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

#include "ctkPluginFrameworkContextPrivate_p.h"

#include "ctkPluginFrameworkPrivate_p.h"
#include "ctkPluginArchive_p.h"

namespace ctk {

  QMutex PluginFrameworkContextPrivate::globalFwLock;
  int PluginFrameworkContextPrivate::globalId = 1;


  PluginFrameworkContextPrivate::PluginFrameworkContextPrivate(
      const PluginFrameworkContext::Properties& initProps)
        : plugins(0), /*services(this),*/ systemPlugin(this),
        storage(this), props(initProps)
  {

    {
      QMutexLocker lock(&globalFwLock);
      id = globalId++;
    }

    log() << "created";
  }

  void PluginFrameworkContextPrivate::init()
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

    PluginFrameworkPrivate* const systemPluginPrivate = systemPlugin.d_func();
    systemPluginPrivate->initSystemPlugin();

    plugins = new Plugins(this);

    plugins->load();

    log() << "inited";

    log() << "Installed plugins:";
    // Use the ordering in the plugin storage to get a sorted list of plugins.
    QList<PluginArchive*> allPAs = storage.getAllPluginArchives();
    for (int i = 0; i < allPAs.size(); ++i)
    {
      PluginArchive* pa = allPAs[i];
      Plugin* p = plugins->getPlugin(pa->getPluginLocation());
      log() << " #" << p->getPluginId() << " " << p->getSymbolicName() << ":"
          << p->getVersion() << " location:" << p->getLocation();
    }
  }

  void PluginFrameworkContextPrivate::uninit()
  {
    log() << "uninit";

    //PluginFrameworkPrivate* const systemPluginPrivate = systemPlugin.d_func();
    //systemPluginPrivate->uninitSystemBundle();

    plugins->clear();
    delete plugins;
    plugins = 0;

    storage.close();

  }

  int PluginFrameworkContextPrivate::getId() const
  {
    return id;
  }

  void PluginFrameworkContextPrivate::checkOurPlugin(Plugin* plugin) const
  {
    PluginPrivate* pp = plugin->d_func();
    if (this != pp->fwCtx)
    {
      throw std::invalid_argument("Plugin does not belong to this framework: " + plugin->getSymbolicName().toStdString());
    }
  }

  QDebug PluginFrameworkContextPrivate::log() const
  {
    QDebug dbg(qDebug());
    dbg << "Framework instance " << getId() << ": ";
    return dbg;
  }

  void PluginFrameworkContextPrivate::resolvePlugin(PluginPrivate* plugin) const
  {
    // TODO

  }


}
