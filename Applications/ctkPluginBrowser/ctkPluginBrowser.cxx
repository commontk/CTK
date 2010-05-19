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

#include "ctkPluginBrowser.h"

#include "ctkPluginTableModel.h"
#include "ctkPluginResourcesTreeModel.h"
#include "ctkQtResourcesTreeModel.h"
#include "ctkServiceReference.h"
#include "ctkPluginConstants.h"

#include <ui_ctkPluginBrowserMainWindow.h>

#include <ctkPluginException.h>
#include <ctkPluginFramework.h>

#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QDirIterator>
#include <QUrl>

namespace ctk {

  PluginBrowser::PluginBrowser(PluginFramework* framework)
    : framework(framework)
  {
    framework->getPluginContext()->connectFrameworkListener(this, SLOT(frameworkEvent(PluginFrameworkEvent)));

    QStringList pluginDirs;
    pluginDirs << qApp->applicationDirPath() + "/Plugins";

    QDirIterator dirIter(pluginDirs.at(0), QDir::Files);
    while(dirIter.hasNext())
    {
      try
      {
        Plugin* plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(dirIter.next()).toString());
        plugin->start(Plugin::START_ACTIVATION_POLICY);
      }
      catch (const PluginException& e)
      {
        qCritical() << e.what();
      }
    }

    framework->start();

    ui.setupUi(this);

    editors = new PluginBrowserEditors(ui.centralwidget);

    QAbstractItemModel* pluginTableModel = new PluginTableModel(framework->getPluginContext(), this);
    ui.pluginsTableView->setModel(pluginTableModel);

    QAbstractItemModel* qtresourcesTreeModel = new QtResourcesTreeModel(this);
    ui.qtResourcesTreeView->setModel(qtresourcesTreeModel);

    connect(ui.pluginsTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(pluginSelected(QModelIndex)));
    connect(ui.pluginsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(pluginDoubleClicked(QModelIndex)));
    connect(ui.pluginResourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dbResourceDoubleClicked(QModelIndex)));
    connect(ui.qtResourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(qtResourceDoubleClicked(QModelIndex)));
  }

  void PluginBrowser::pluginSelected(const QModelIndex &index)
  {
    QVariant v = index.data(Qt::UserRole);

    Plugin* plugin = framework->getPluginContext()->getPlugin(v.toLongLong());

    if (!plugin) return;

    QAbstractItemModel* oldModel = ui.pluginResourcesTreeView->model();
    ui.pluginResourcesTreeView->setModel(new PluginResourcesTreeModel(plugin, this));
    if (oldModel) oldModel->deleteLater();;
  }

  void PluginBrowser::pluginDoubleClicked(const QModelIndex& index)
  {
    long pluginId = index.data(Qt::UserRole).toLongLong();
    Plugin* plugin = framework->getPluginContext()->getPlugin(pluginId);

    QByteArray mfContent = plugin->getResource("/META-INF/MANIFEST.MF");
    QString location = QString("/") + plugin->getSymbolicName() + "/META-INF/MANIFEST.MF";
    editors->openEditor(location, mfContent, location + " [cached]");

    QList<ServiceReference*> serviceRefs = plugin->getPluginContext()->getServiceReferences("");
    QListIterator<ServiceReference*> it(serviceRefs);
    while (it.hasNext())
    {
      ServiceReference* ref = it.next();
      qDebug() << "Service from" << ref->getPlugin()->getSymbolicName() << ":" << ref->getPropertyKeys();
      qDebug() << "Object Classes:" << ref->getProperty(PluginConstants::OBJECTCLASS).toStringList();
    }
  }

  void PluginBrowser::qtResourceDoubleClicked(const QModelIndex& index)
  {

  }

  void PluginBrowser::dbResourceDoubleClicked(const QModelIndex& index)
  {
    QString resPath = index.data(Qt::UserRole).toString();
    if (resPath.isEmpty() || resPath.endsWith('/')) return;

    qDebug() << "Trying to open: " << resPath;

    QModelIndex pluginIndex = ui.pluginsTableView->selectionModel()->selectedIndexes().first();
    long pluginId = pluginIndex.data(Qt::UserRole).toLongLong();

    Plugin* plugin = framework->getPluginContext()->getPlugin(pluginId);

    QByteArray resContent = plugin->getResource(resPath);
    QString location = QString("/") + plugin->getSymbolicName() + resPath;
    editors->openEditor(location, resContent, location + " [cached]");
  }

  void PluginBrowser::frameworkEvent(const PluginFrameworkEvent& event)
  {
    qDebug() << "FrameworkEvent: [" << event.getPlugin()->getSymbolicName() << "]" << event.getErrorString();
  }


}
