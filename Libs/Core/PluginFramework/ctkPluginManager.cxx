/*
 * ctkPluginManager.cxx
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#include "ctkPluginManager.h"

#include <QDirIterator>
#include <QDebug>
#include <QLibrary>

namespace ctk {

class PluginManagerPrivate
{
public:

  QList<QString> pluginPaths;
};

PluginManager::PluginManager()
: d_ptr(new PluginManagerPrivate())
{

}

PluginManager::~PluginManager()
{
  Q_D(PluginManager);
  delete d;
}

void PluginManager::addSearchPath(const QString & searchPath)
{
  Q_D(PluginManager);
  d->pluginPaths.push_back(searchPath);
}

void PluginManager::startAllPlugins()
{
  Q_D(PluginManager);
  QDirIterator it(d->pluginPaths.front(), QDir::Files);
  while (it.hasNext()) {
       QString libName(it.next());
       QLibrary lib(libName);
       QFileInfo fileInfo(libName);
       QString libBaseName(fileInfo.baseName());
       if (libBaseName.startsWith("lib"))
       {
         libBaseName.remove(0, 3);
       }
       qDebug() << libBaseName;
       lib.load();
       qDebug() << "lib loaded: " << lib.isLoaded();
       QString xyz = QString(":/") + libBaseName + "/servicedescriptor.xml";
       //QString xyz = ":/servicedescriptor.xml";
       qDebug() << "resource string: " << xyz;
       QFile serviceDescriptor(xyz);
       qDebug() << "file exists: " << serviceDescriptor.exists();
       qDebug() << "open returns:" << serviceDescriptor.open(QIODevice::ReadOnly);
       qDebug() << "file open: " << serviceDescriptor.isOpen();
       qDebug() << "file is readable: " << serviceDescriptor.isReadable();
       QByteArray serviceBA = serviceDescriptor.readAll();
       qDebug() << serviceBA;
       lib.unload();
  }

}

}
