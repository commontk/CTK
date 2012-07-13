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

#include <QUrl>

#include "ctkPlugin_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginException.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPlugins_p.h"
#include "ctkVersionRange_p.h"

#include <stdexcept>
#include <iostream>

//----------------------------------------------------------------------------
void ctkPlugins::checkIllegalState() const
{
  if (!fwCtx)
  {
    throw ctkIllegalStateException("This framework instance is not active");
  }
}

//----------------------------------------------------------------------------
ctkPlugins::ctkPlugins(ctkPluginFrameworkContext* fw)
{
  fwCtx = fw;
  plugins.insert(fw->systemPlugin->getLocation(), fw->systemPlugin);
}

//----------------------------------------------------------------------------
void ctkPlugins::clear()
{
  QWriteLocker lock(&pluginsLock);
  plugins.clear();
  fwCtx = 0;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPlugins::install(const QUrl& location, QIODevice* in)
{
  checkIllegalState();

  QSharedPointer<ctkPlugin> res;
  {
    QMutexLocker lock(&objectLock);

    QHash<QString, QSharedPointer<ctkPlugin> >::const_iterator it = plugins.find(location.toString());
    if (it != plugins.end())
    {
      return it.value();
    }

    // install new plugin
    QSharedPointer<ctkPluginArchive> pa;
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
          throw ctkRuntimeException(QString("Unsupported url scheme: ") + location.scheme());
        }
        else
        {
          localPluginPath = location.toLocalFile();
        }
      }
      else
      {
        //TODO copy the QIODevice to a local cache
      }

      pa = fwCtx->storage->insertPlugin(location, localPluginPath);

      res = QSharedPointer<ctkPlugin>(new ctkPlugin());
      res->init(res, fwCtx, pa);
      plugins.insert(location.toString(), res);
    }
    catch (const ctkException& e)
    {
      if (!pa.isNull())
      {
        pa->purge();
      }
      //      if (dynamic_cast<const SecurityException&>(e)) {
      //        throw;
      //      }
      //      else
      //      {
      throw ctkPluginException("Failed to install plugin",
                               ctkPluginException::UNSPECIFIED, e);
      //      }
    }
    catch (...)
    {
      throw ctkPluginException("Failed to install plugin", ctkPluginException::UNSPECIFIED);
    }
  }

  fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::INSTALLED, res));
  return res;
}

//----------------------------------------------------------------------------
void ctkPlugins::remove(const QUrl& location)
{
  QWriteLocker lock(&pluginsLock);
  plugins.remove(location.toString());
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPlugins::getPlugin(int id) const
{
  checkIllegalState();

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

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPlugins::getPlugin(const QString& location) const
{
  checkIllegalState();

  QReadLocker lock(&pluginsLock);
  QHash<QString, QSharedPointer<ctkPlugin> >::const_iterator it = plugins.find(location);
  if (it != plugins.end()) return it.value();
  return QSharedPointer<ctkPlugin>(0);
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPlugins::getPlugin(const QString& name, const ctkVersion& version) const
{
  checkIllegalState();

  {
    QReadLocker lock(&pluginsLock);

    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      QSharedPointer<ctkPlugin> plugin = it.next().value();
      if ((name == plugin->getSymbolicName()) && (version == plugin->getVersion()))
      {
        return plugin;
      }
    }
  }
  return QSharedPointer<ctkPlugin>(0);
}

//----------------------------------------------------------------------------
QList<QSharedPointer<ctkPlugin> > ctkPlugins::getPlugins() const
{
  checkIllegalState();

  {
    QReadLocker lock(&pluginsLock);
    return plugins.values();
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
QList<ctkPlugin*> ctkPlugins::getPlugins(const QString& name, const ctkVersionRange& range) const
{
  checkIllegalState();

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

//----------------------------------------------------------------------------
QList<QSharedPointer<ctkPlugin> > ctkPlugins::getActivePlugins() const
{
  checkIllegalState();

  QList<QSharedPointer<ctkPlugin> > slist;
  {
    QReadLocker lock(&pluginsLock);
    QHashIterator<QString, QSharedPointer<ctkPlugin> > it(plugins);
    while (it.hasNext())
    {
      QSharedPointer<ctkPlugin> plugin = it.next().value();
      ctkPlugin::State s = plugin->getState();
      if (s == ctkPlugin::ACTIVE || s == ctkPlugin::STARTING) {
        slist.push_back(plugin);
      }
    }
  }
  return slist;
}

//----------------------------------------------------------------------------
void ctkPlugins::load()
{
  QList<QSharedPointer<ctkPluginArchive> > pas = fwCtx->storage->getAllPluginArchives();
  QListIterator<QSharedPointer<ctkPluginArchive> > it(pas);

  {
    QMutexLocker lock(&objectLock);
    while (it.hasNext())
    {
      QSharedPointer<ctkPluginArchive> pa = it.next();
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
        std::cerr << e.what() << std::endl;
      }
    }
  }
}

//----------------------------------------------------------------------------
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
        pp->fwCtx->listeners.frameworkError(pp->q_func(), pe);
      }
    }
  }
}
