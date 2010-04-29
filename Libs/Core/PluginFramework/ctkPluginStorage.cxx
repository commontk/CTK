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

#include "ctkPluginStorage_p.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

// CTK includes
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContextPrivate_p.h"

namespace ctk {

  PluginStorage::PluginStorage(PluginFrameworkContextPrivate* framework)
    : nextFreeId(1), framework(framework)
  {
//    // See if we have a storage database
//    bundlesDir = Util.getFileStorage(framework, "bs");
//    if (bundlesDir == null) {
//      throw new RuntimeException("No bundle storage area available!");
//    }
//    // Restore all saved bundles
//    String [] list = bundlesDir.list();
//    for (int i = 0; list != null & i < list.length; i++) {
//      long id;
//      try {
//        id = Long.parseLong(list[i]);
//      } catch (NumberFormatException e) {
//        continue;
//      }
//      if (id == 0) {
//        System.err.println("Saved bundle with illegal id 0 is ignored.");
//      }
//      int pos = find(id);
//      if (pos < archives.size() && ((BundleArchive)archives.get(pos)).getBundleId() == id) {
//        System.err.println("There are two bundle directories with id: " + id);
//        break;
//      }
//      FileTree dir = new FileTree(bundlesDir, list[i]);
//      if (dir.isDirectory()) {
//        try {
//          boolean bUninstalled = BundleArchiveImpl.isUninstalled(dir);
//          if(bUninstalled) {
//            // silently remove any bundle marked as uninstalled
//            dir.delete();
//          } else {
//            BundleArchive ba = new BundleArchiveImpl(this, dir, id);
//            archives.add(pos, ba);
//          }
//          if (id >= nextFreeId) {
//            nextFreeId = id + 1;
//          }
//        } catch (Exception e) {
//          dir.delete();
//          System.err.println("Removed corrupt bundle dir (" + e.getMessage() + "): " + dir);
//        }
//      }
//    }
  }

  PluginArchive* PluginStorage::insertPlugin(const QString& location, QIODevice* is)
  {
    long id = nextFreeId++;

    //TODO check the db if the plugin is already cached
    //...

    // work-around for now without the cache:
    // load the plugin to have access to Qt resources
    QPluginLoader pluginLoader;
    QString resourcePrefix;
    if (!is)
    {
      QUrl url(location);
      pluginLoader.setFileName(url.path());
      QFileInfo fileInfo(url.path());
      resourcePrefix = fileInfo.baseName();
      if (resourcePrefix.startsWith("lib"))
      {
        resourcePrefix = resourcePrefix.mid(3);
      }
      resourcePrefix.replace("_", ".");

      qDebug() << QString("Loading plugin") << pluginLoader.fileName() << ":" << pluginLoader.load();
    }
    else
    {
      resourcePrefix = location;
    }

    PluginArchive* pa = new PluginArchive(this, is, location, id, resourcePrefix);
    archives.push_back(pa);

    pluginLoader.unload();
    return pa;
  }

  PluginArchive* PluginStorage::updatePluginArchive(PluginArchive* old, QIODevice* is)
  {
    //return new BundleArchiveImpl((BundleArchiveImpl)old, is);
    return 0;
  }

  void PluginStorage::replacePluginArchive(PluginArchive* oldPA, PluginArchive* newPA)
  {
//    int pos;
//    long id = oldBA.getBundleId();
//    synchronized (archives) {
//      pos = find(id);
//      if (pos >= archives.size() || archives.get(pos) != oldBA) {
//        throw new Exception("replaceBundleJar: Old bundle archive not found, pos=" + pos);
//      }
//      archives.set(pos, newBA);
//    }
  }

  QList<PluginArchive*> PluginStorage::getAllPluginArchives() const {
    return archives;
  }

  QList<QString> PluginStorage::getStartOnLaunchPlugins() {
    QList<QString> res;
    QListIterator<PluginArchive*> i(archives);
    while(i.hasNext())
    {
      PluginArchive* ba = i.next();
      if (ba->getAutostartSetting() != -1)
      {
        res.push_back(ba->getPluginLocation());
      }
    }
    return res;
  }

  void PluginStorage::close()
  {
//    for (Iterator i = archives.iterator(); i.hasNext(); ) {
//      BundleArchive ba = (BundleArchive) i.next();
//      ba.close();
//      i.remove();
//    }
//    framework = null;
//    bundlesDir = null;
  }

  bool PluginStorage::removeArchive(const PluginArchive* ba) {
//    synchronized (archives) {
//      int pos = find(ba.getBundleId());
//      if (pos < archives.size() && archives.get(pos) == ba) {
//        archives.remove(pos);
//        return true;
//      } else {
//        return false;
//      }
//    }
  }

  int PluginStorage::find(long id) {
    int lb = 0;
//    int ub = archives.size() - 1;
//    int x = 0;
//    while (lb < ub) {
//      x = (lb + ub) / 2;
//      long xid = ((BundleArchive)archives.get(x)).getBundleId();
//      if (id == xid) {
//        return x;
//      } else if (id < xid) {
//        ub = x;
//      } else {
//        lb = x+1;
//      }
//    }
//    if (lb < archives.size() && ((BundleArchive)archives.get(lb)).getBundleId() < id) {
//      return lb + 1;
//    }
    return lb;
  }

  QString PluginStorage::getPluginResource(const QString& res) const
  {
    //TODO implement sql cache
    qDebug() << "Getting resource:" << res;
    QFileInfo info(res);
    if (info.exists() && info.isReadable() && info.isFile())
      return res;
    return QString();
  }

  QStringList PluginStorage::findResourcesPath(const QString& path) const
  {
    //TODO implement sql cache
    QFileInfo info(path);
    if (info.isDir() && info.isReadable())
    {
      return info.absoluteDir().entryList();
    }

    return QStringList();
  }

}
