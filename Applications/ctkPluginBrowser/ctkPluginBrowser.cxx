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

#include <ui_ctkPluginBrowserMainWindow.h>

#include <PluginFramework/ctkPluginException.h>
#include <PluginFramework/ctkPluginFramework.h>

#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QDirIterator>
#include <QUrl>

namespace ctk {

  PluginBrowser::PluginBrowser(PluginFramework* framework)
    : framework(framework)
  {

    QStringList pluginDirs;
    pluginDirs << qApp->applicationDirPath() + "/Plugins";

    QDirIterator dirIter(pluginDirs.at(0), QDir::Files);
    while(dirIter.hasNext())
    {
      try
      {
        framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(dirIter.next()).toString());
      }
      catch (const PluginException& e)
      {
        qCritical() << e.what();
      }
    }

    framework->start();

    ui.setupUi(this);

    QAbstractItemModel* pluginTableModel = new PluginTableModel(framework->getPluginContext(), this);
    ui.pluginsTableView->setModel(pluginTableModel);

    QAbstractItemModel* qtresourcesTreeModel = new QtResourcesTreeModel(this);
    ui.qtResourcesTreeView->setModel(qtresourcesTreeModel);

    connect(ui.pluginsTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(pluginSelected(QModelIndex)));

  }

  void PluginBrowser::pluginSelected(const QModelIndex &index)
  {
    QVariant v = index.data(Qt::UserRole);
    qDebug() << "Selected plugin:" << v;

    Plugin* plugin = framework->getPluginContext()->getPlugin(v.toInt());

    if (!plugin) return;

    QAbstractItemModel* oldModel = ui.pluginResourcesTreeView->model();
    ui.pluginResourcesTreeView->setModel(new PluginResourcesTreeModel(plugin, this));
    if (oldModel) oldModel->deleteLater();;
  }


}
