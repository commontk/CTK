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

#include "ctkPlugins_p.h"

#include "ctkPluginPrivate_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginException.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkVersionRange_p.h"

#include <stdexcept>
#include <iostream>

#include <QUrl>


ctkPlugins::ctkPlugins(ctkPluginFrameworkContext* fw)
{
  fwCtx = fw;
  plugins.insert(fw->systemPlugin->getLocation(), fw->systemPlugin);
}

void ctkPlugins::clear()
{
  QWriteLocker lock(&pluginsLock);
  plugins.clear();
  fwCtx = 0;
}

QSharedPointer<ctkPlugin> ctkPlugins::install(const QUrl& location, QIODevice* in)
{
  if (!fwCtx)
  { // This ctkPlugins instance has been closed!
    throw std::logic_error("ctkPlugins::install(location, inputStream) called on closed plugins object.");
  }

  {
    QWriteLocker lock(&pluginsLock);

    QHash<QString, QSharedPointer<ctkPlugin> >::const_iterator it = plugins.find(location.toString());
    if (it != plugins.end())
    {
      return it.value();
    }

    // install new plugin
    ctkPluginArchive* pa = 0;
    QString localPluginPath;
    try
    {
      if (!in)
      {
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

      pa = fwCtx->storage->insertPlugin(location, localPluginPath);

      QSharedPointer<ctkPlugin> res(new ctkPlugin());
      res->init(res, fwCtx, pa);
      plugins.insert(location.toString(), res);

      fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::INSTALLED, res));

      return res;
    }
    catch (const std::exception& e)
    {
      if (pa)
      {
        pa->purge();
      }
      //      if (dynamic_cast<const SecurityException&>(e)) {
      //        throw;
      //      }
      //      else
      //      {
      throw ctkPluginException(QString("Failed to install plugin: ") + QString(e.what()),
                               ctkPluginException::UNSPECIFIED, &e);
      //      }
    }
  }

}

void ctkPlugins::remove(const QUrl& location)
{
  QWriteLocker lock(&pluginsLock);
  plugins.remove(location.toString());
}

QSharedPointer<ctkPlugin> ctkPlugins::getPlugin(int id) const
{
  if (!fwCtx)
  { // This plugins instance has been closed!
    throw std::logic_error("ctkPlugins::getPlugin(id) called on closed plugins object.");
  }

  {
    QReadLocker lock(&pluginsLock);

    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      QSharedPointer<ctkPlugin> plugin = it.next().value();
      if (plugin->getPluginId() == id)
      {
        return plugin;
      }
    }
  }
  return QSharedPointer<ctkPlugin>();
}

ctkPlugin* ctkPlugins::getPlugin(const QString& location) const
{
  if (!fwCtx)
  { // This plugins instance has been closed!
    throw std::logic_error("ctkPlugins::getPlugin(location) called on closed plugins object.");
  }

  QReadLocker lock(&pluginsLock);
  QHash<QString, QSharedPointer<ctkPlugin> >::const_iterator it = plugins.find(location);
  if (it != plugins.end()) return it.value().data();
  return 0;
}

ctkPlugin* ctkPlugins::getPlugin(const QString& name, const ctkVersion& version) const
{
  if (!fwCtx)
  { // This ctkPlugins instance has been closed!
    throw std::logic_error("ctkPlugins::getPlugin(name, version) called on closed plugins object.");
  }

  {
    QReadLocker lock(&pluginsLock);

    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      QSharedPointer<ctkPlugin> plugin = it.next().value();
      if ((name == plugin->getSymbolicName()) && (version == plugin->getVersion()))
      {
        return plugin.data();
      }
    }
  }
  return 0;
}

QList<QSharedPointer<ctkPlugin> > ctkPlugins::getPlugins() const
{
  if (!fwCtx)
  { // This plugins instance has been closed!
    throw std::logic_error("ctkPlugins::getPlugins() called on closed plugins object.");
  }

  {
    QReadLocker lock(&pluginsLock);
    return plugins.values();
  }
}

QList<ctkPlugin*> ctkPlugins::getPlugins(const QString& name) const
{
  QList<ctkPlugin*> res;

  {
    QReadLocker lock(&pluginsLock);
    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      ctkPlugin* plugin = it.next().value().data();
      if (name == plugin->getSymbolicName())
      {
        res.push_back(plugin);
      }
    }
  }

  return res;
}

QList<ctkPlugin*> ctkPlugins::getPlugins(const QString& name, const ctkVersionRange& range) const
{
  if (!fwCtx)
  { // This plugins instance has been closed!
    throw std::logic_error("ctkPlugins::getPlugins(name, versionRange) called on closed plugins object.");
  }

  QList<ctkPlugin*> pluginsWithName = getPlugins(name);
  QList<ctkPlugin*> res;

  QListIterator<ctkPlugin*> it(pluginsWithName);
  while (it.hasNext()) {
    ctkPlugin* plugin = it.next();
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

QList<ctkPlugin*> ctkPlugins::getActivePlugins() const
{
  if (!fwCtx)
  { // This plugins instance has been closed!
    throw std::logic_error("ctkPlugins::getActivePlugins() called on closed plugins object.");
  }

  QList<ctkPlugin*> slist;
  {
    QReadLocker lock(&pluginsLock);
    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      QSharedPointer<ctkPlugin> plugin = it.next().value();
      ctkPlugin::State s = plugin->getState();
      if (s == ctkPlugin::ACTIVE || s == ctkPlugin::STARTING) {
        slist.push_back(plugin.data());
      }
    }
  }
  return slist;
}

void ctkPlugins::load()
{
  QList<ctkPluginArchive*> pas = fwCtx->storage->getAllPluginArchives();
  QListIterator<ctkPluginArchive*> it(pas);

  {
    QWriteLocker lock(&pluginsLock);
    while (it.hasNext())
    {
      ctkPluginArchive* pa = it.next();
      try
      {
        QSharedPointer<ctkPlugin> plugin(new ctkPlugin());
        plugin->init(plugin, fwCtx, pa);
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

void ctkPlugins::startPlugins(const QList<ctkPlugin*>& slist) const
{
  // Sort in start order
  // Resolve first to avoid dead lock
  QListIterator<ctkPlugin*> it(slist);
  while (it.hasNext())
  {
    ctkPlugin* plugin = it.next();
    ctkPluginPrivate* pp = plugin->d_func();
    pp->getUpdatedState();
  }

  it.toFront();
  while (it.hasNext())
  {
    ctkPlugin* plugin = it.next();
    ctkPluginPrivate* pp = plugin->d_func();
    if (pp->getUpdatedState() == ctkPlugin::RESOLVED)
    {
      try
      {
        plugin->start(0);
      }
      catch (const ctkPluginException& pe)
      {
        pp->fwCtx->listeners.frameworkError(plugin, pe);
      }
    }
  }
}
