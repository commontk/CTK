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

#include "ctkPlugins_p.h"

#include "ctkPluginPrivate_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginException.h"
#include "ctkPluginFrameworkContextPrivate_p.h"
#include "ctkVersionRange_p.h"

#include <stdexcept>
#include <iostream>

#include <QUrl>

namespace ctk {

  Plugins::Plugins(PluginFrameworkContextPrivate* fw) {
    fwCtx = fw;
    plugins.insert(fw->systemPlugin.getLocation(), &fw->systemPlugin);
  }

  void Plugins::clear()
  {
    QWriteLocker lock(&pluginsLock);
    plugins.clear();
    fwCtx = 0;
  }

  Plugin* Plugins::install(const QUrl& location, QIODevice* in)
  {
    if (!fwCtx)
    { // This Plugins instance has been closed!
      throw std::logic_error("Plugins::install(location, inputStream) called on closed plugins object.");
    }

    {
      QWriteLocker lock(&pluginsLock);

      QHash<QString, Plugin*>::const_iterator it = plugins.find(location.toString());
      if (it != plugins.end()) {
        return it.value();
      }

      // install new plugin
      PluginArchive* pa = 0;
      QString localPluginPath;
      try {
        if (!in) {
          // extract the input stream from the given location
          

//          //TODO Support for http proxy authentication
//          //TODO put in update as well
//          String auth = fwCtx.props.getProperty("http.proxyAuth");
//          if (auth != null && !"".equals(auth)) {
//            if ("http".equals(url.getProtocol()) ||
//                "https".equals(url.getProtocol())) {
//              String base64 = Util.base64Encode(auth);
//              conn.setRequestProperty("Proxy-Authorization",
//                                      "Basic " + base64);
//            }
//          }
//          // Support for http basic authentication
//          String basicAuth = fwCtx.props.getProperty("http.basicAuth");
//          if (basicAuth != null && !"".equals(basicAuth)) {
//            if ("http".equals(url.getProtocol()) ||
//                "https".equals(url.getProtocol())) {
//              String base64 = Util.base64Encode(basicAuth);
//              conn.setRequestProperty("Authorization",
//                                      "Basic " +base64);
//            }
//          }

          if (location.scheme() != "file")
          {
            throw std::runtime_error(std::string("Unsupported url scheme: ") + qPrintable(location.scheme()));
          }
          else
          {
            qDebug() << QString("Trying to install file:") << location.path();
            localPluginPath = location.toLocalFile();
          }
        } 
        else 
        {
          //TODO copy the QIODevice to a local cache
        }

        pa = fwCtx->storage.insertPlugin(location, localPluginPath);

        Plugin* res = new Plugin(fwCtx, pa);
        plugins.insert(location.toString(), res);

        //TODO send event
        //fwCtx.listeners.bundleChanged(new BundleEvent(BundleEvent.INSTALLED, b));

        return res;
      }
      catch (const std::exception& e)
      {
        if (pa) {
          pa->purge();
        }
  //      if (dynamic_cast<const SecurityException&>(e)) {
  //        throw;
  //      }
  //      else
  //      {
        throw PluginException(QString("Failed to install plugin: ") + QString(e.what()),
                                PluginException::UNSPECIFIED, e);
  //      }
      }
    }

  }

  void Plugins::remove(const QUrl& location)
  {
    QWriteLocker lock(&pluginsLock);
    delete plugins.take(location.toString());
  }

  Plugin* Plugins::getPlugin(int id) const
  {
    if (!fwCtx)
    { // This plugins instance has been closed!
      throw std::logic_error("Plugins::getPlugin(id) called on closed plugins object.");
    }

    {
      QReadLocker lock(&pluginsLock);

      QHashIterator<QString, Plugin*> it(plugins);
      while (it.hasNext())
      {
        Plugin* plugin = it.next().value();
        if (plugin->getPluginId() == id) {
          return plugin;
        }
      }
    }
    return 0;
  }

  Plugin* Plugins::getPlugin(const QString& location) const {
    if (!fwCtx)
    { // This plugins instance has been closed!
      throw std::logic_error("Plugins::getPlugin(location) called on closed plugins object.");
    }

    QReadLocker lock(&pluginsLock);
    QHash<QString, Plugin*>::const_iterator it = plugins.find(location);
    if (it != plugins.end()) return it.value();
    return 0;
  }

  Plugin* Plugins::getPlugin(const QString& name, const Version& version) const
  {
    if (!fwCtx)
    { // This Plugins instance has been closed!
      throw std::logic_error("Plugins::getPlugin(name, version) called on closed plugins object.");
    }

    {
      QReadLocker lock(&pluginsLock);

      QHashIterator<QString, Plugin*> it(plugins);
      while (it.hasNext())
      {
        Plugin* plugin = it.next().value();
        if ((name == plugin->getSymbolicName()) && (version == plugin->getVersion()))
        {
          return plugin;
        }
      }
    }
    return 0;
  }

  QList<Plugin*> Plugins::getPlugins() const
  {
    if (!fwCtx)
    { // This plugins instance has been closed!
      throw std::logic_error("Plugins::getPlugins() called on closed plugins object.");
    }

    {
      QReadLocker lock(&pluginsLock);
      return plugins.values();
    }
  }

  QList<Plugin*> Plugins::getPlugins(const QString& name) const
  {
    QList<Plugin*> res;

    {
      QReadLocker lock(&pluginsLock);
      QHashIterator<QString, Plugin*> it(plugins);
      while (it.hasNext())
      {
        Plugin* plugin = it.next().value();
        if (name == plugin->getSymbolicName())
        {
          res.push_back(plugin);
        }
      }
    }

    return res;
  }

  QList<Plugin*> Plugins::getPlugins(const QString& name, const VersionRange& range) const {
    if (!fwCtx)
    { // This plugins instance has been closed!
      throw std::logic_error("Plugins::getPlugins(name, versionRange) called on closed plugins object.");
    }

    QList<Plugin*> pluginsWithName = getPlugins(name);
    QList<Plugin*> res;

    QListIterator<Plugin*> it(pluginsWithName);
    while (it.hasNext()) {
      Plugin* plugin = it.next();
      if (range.withinRange(plugin->getVersion()))
      {
        int j = res.size();
        while (--j >= 0)
        {
          if (plugin->getVersion().compare(res.at(j)->getVersion()) <= 0)
          {
            break;
          }
        }
        res.insert(j + 1, plugin);
      }
    }

    return res;
  }

  QList<Plugin*> Plugins::getActivePlugins() const {
    if (!fwCtx)
    { // This plugins instance has been closed!
      throw std::logic_error("Plugins::getActivePlugins() called on closed plugins object.");
    }

    QList<Plugin*> slist;
    {
      QReadLocker lock(&pluginsLock);
      QHashIterator<QString, Plugin*> it(plugins);
      while (it.hasNext())
      {
        Plugin* plugin = it.next().value();
        Plugin::State s = plugin->getState();
        if (s == Plugin::ACTIVE || s == Plugin::STARTING) {
          slist.push_back(plugin);
        }
      }
    }
    return slist;
  }

  void Plugins::load() {
    QList<PluginArchive*> pas = fwCtx->storage.getAllPluginArchives();
    QListIterator<PluginArchive*> it(pas);

    {
      QWriteLocker lock(&pluginsLock);
      while (it.hasNext())
      {
        PluginArchive* pa = it.next();
        try
        {
          Plugin* plugin = new Plugin(fwCtx, pa);
          plugins.insert(pa->getPluginLocation().toString(), plugin);
        }
        catch (const std::exception& e)
        {
          pa->setAutostartSetting(-1); // Do not start on launch
          pa->setStartLevel(-2); // Mark as uninstalled

          std::cerr << "Error: Failed to load bundle "
                    << pa->getPluginId()
                    << " ("  << qPrintable(pa->getPluginLocation().toString())  << ")"
                    << " uninstalled it!\n";
          std::cerr << e.what();
        }
      }
    }
  }

  void Plugins::startPlugins(const QList<Plugin*>& slist) const {
    // Sort in start order
    // Resolve first to avoid dead lock
    QListIterator<Plugin*> it(slist);
    while (it.hasNext())
    {
      Plugin* plugin = it.next();
      PluginPrivate* pp = plugin->d_func();
      pp->getUpdatedState();
    }

    it.toFront();
    while (it.hasNext())
    {
      Plugin* plugin = it.next();
      PluginPrivate* pp = plugin->d_func();
      if (pp->getUpdatedState() == Plugin::RESOLVED)
      {
        try
        {
          plugin->start(0);
        }
        catch (const PluginException& pe)
        {
          pp->fwCtx->listeners.frameworkError(plugin, pe);
        }
      }
    }
  }

}
