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

#include "ctkPluginBrowser.h"

#include "ctkPluginTableModel.h"
#include "ctkPluginResourcesTreeModel.h"
#include "ctkQtResourcesTreeModel.h"
#include "ctkServiceReference.h"
#include "ctkServiceException.h"
#include "ctkPluginConstants.h"

#include <ui_ctkPluginBrowserMainWindow.h>

#include <ctkConfig.h>
#include <ctkPluginException.h>
#include <ctkPluginFramework.h>
#include <ctkPluginContext.h>

#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QDirIterator>
#include <QUrl>
#include <QSettings>
#include <QCloseEvent>

#define SETTINGS_WND_GEOM "mainwindow.geom"
#define SETTINGS_WND_STATE "mainwindow.state"

ctkPluginBrowser::ctkPluginBrowser(ctkPluginFramework* framework)
  : framework(framework)
{
  pluginEventTypeToString[ctkPluginEvent::INSTALLED] = "Installed";
  pluginEventTypeToString[ctkPluginEvent::LAZY_ACTIVATION] = "Lazy Activation";
  pluginEventTypeToString[ctkPluginEvent::RESOLVED] = "Resolved";
  pluginEventTypeToString[ctkPluginEvent::STARTED] = "Started";
  pluginEventTypeToString[ctkPluginEvent::STARTING] = "Starting";
  pluginEventTypeToString[ctkPluginEvent::STOPPED] = "Stopped";
  pluginEventTypeToString[ctkPluginEvent::STOPPING] = "Stopping";
  pluginEventTypeToString[ctkPluginEvent::UNINSTALLED] = "Uninstalled";
  pluginEventTypeToString[ctkPluginEvent::UNRESOLVED] = "Unresolved";
  pluginEventTypeToString[ctkPluginEvent::UPDATED] = "Updated";

  framework->getPluginContext()->connectFrameworkListener(this, SLOT(frameworkEvent(ctkPluginFrameworkEvent)));
  framework->getPluginContext()->connectPluginListener(this, SLOT(pluginEvent(ctkPluginEvent)));
  framework->getPluginContext()->connectServiceListener(this, "serviceEvent");

  QStringList pluginDirs;
#ifdef CMAKE_INTDIR
  pluginDirs << CTK_PLUGIN_DIR CMAKE_INTDIR "/";
#else
  pluginDirs << CTK_PLUGIN_DIR;
#endif

  QStringListIterator dirIt(pluginDirs);
  while (dirIt.hasNext())
  {
    QApplication::addLibraryPath(dirIt.next());
  }

  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginDirs.at(0), libFilter, QDir::Files);
  while(dirIter.hasNext())
  {
    try
    {
      framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(dirIter.next()).toString());
      //plugin->start(ctkPlugin::START_ACTIVATION_POLICY);
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  framework->start();

  ui.setupUi(this);

  tabifyDockWidget(ui.qtResourcesDockWidget, ui.pluginResourcesDockWidget);

  editors = new ctkPluginBrowserEditors(ui.centralwidget);

  QAbstractItemModel* pluginTableModel = new ctkPluginTableModel(framework->getPluginContext(), this);
  ui.pluginsTableView->setModel(pluginTableModel);

  QAbstractItemModel* qtresourcesTreeModel = new ctkQtResourcesTreeModel(this);
  ui.qtResourcesTreeView->setModel(qtresourcesTreeModel);

  connect(ui.pluginsTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(pluginSelected(QModelIndex)));
  connect(ui.pluginsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(pluginDoubleClicked(QModelIndex)));
  connect(ui.pluginResourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dbResourceDoubleClicked(QModelIndex)));
  connect(ui.qtResourcesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(qtResourceDoubleClicked(QModelIndex)));

  startPluginNowAction = new QAction(QIcon(":/pluginbrowser/images/run-now.png"), "Start Plugin (ignore activation policy)", this);
  startPluginAction = new QAction(QIcon(":/pluginbrowser/images/run.png"), "Start Plugin", this);
  stopPluginAction = new QAction(QIcon(":/pluginbrowser/images/stop.png"), "Stop Plugin", this);

  connect(startPluginNowAction, SIGNAL(triggered()), this, SLOT(startPluginNow()));
  connect(startPluginAction, SIGNAL(triggered()), this, SLOT(startPlugin()));
  connect(stopPluginAction, SIGNAL(triggered()), this, SLOT(stopPlugin()));

  startPluginNowAction->setEnabled(false);
  startPluginAction->setEnabled(false);
  stopPluginAction->setEnabled(false);

  ui.pluginToolBar->addAction(startPluginNowAction);
  ui.pluginToolBar->addAction(startPluginAction);
  ui.pluginToolBar->addAction(stopPluginAction);

  QSettings settings;
  if(settings.contains(SETTINGS_WND_GEOM))
  {
    this->restoreGeometry(settings.value(SETTINGS_WND_GEOM).toByteArray());
  }
  if (settings.contains(SETTINGS_WND_STATE))
  {
    this->restoreState(settings.value(SETTINGS_WND_STATE).toByteArray());
  }
}

void ctkPluginBrowser::pluginSelected(const QModelIndex &index)
{
  QVariant v = index.data(Qt::UserRole);

  QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->getPlugin(v.toLongLong());
  if (!plugin) return;  
  updatePluginToolbar(plugin);

  QAbstractItemModel* oldModel = ui.pluginResourcesTreeView->model();
  ui.pluginResourcesTreeView->setModel(new ctkPluginResourcesTreeModel(plugin, this));
  if (oldModel) oldModel->deleteLater();;
}

void ctkPluginBrowser::updatePluginToolbar(QSharedPointer<ctkPlugin> plugin)
{
  startPluginNowAction->setEnabled(false);
  startPluginAction->setEnabled(false);
  stopPluginAction->setEnabled(false);

  if (!plugin) return;

  const ctkPlugin::States startStates = ctkPlugin::INSTALLED | ctkPlugin::RESOLVED | ctkPlugin::STOPPING;
  const ctkPlugin::States stopStates = ctkPlugin::STARTING | ctkPlugin::ACTIVE;
  if (startStates.testFlag(plugin->getState()))
  {
    startPluginNowAction->setEnabled(true);
    startPluginAction->setEnabled(true);
  }

  if (stopStates.testFlag(plugin->getState()))
  {
    stopPluginAction->setEnabled(true);
  }
}

void ctkPluginBrowser::pluginDoubleClicked(const QModelIndex& index)
{
  long pluginId = index.data(Qt::UserRole).toLongLong();
  QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->getPlugin(pluginId);

  QByteArray mfContent = plugin->getResource("/META-INF/MANIFEST.MF");
  QString location = QString("/") + plugin->getSymbolicName() + "/META-INF/MANIFEST.MF";
  editors->openEditor(location, mfContent, location + " [cached]");

  QList<ctkServiceReference> serviceRefs = plugin->getPluginContext()->getServiceReferences("");
  QListIterator<ctkServiceReference> it(serviceRefs);
  while (it.hasNext())
  {
    ctkServiceReference ref(it.next());
    qDebug() << "Service from" << ref.getPlugin()->getSymbolicName() << ":" << ref.getPropertyKeys();
    qDebug() << "Object Classes:" << ref.getProperty(ctkPluginConstants::OBJECTCLASS).toStringList();
  }
}

void ctkPluginBrowser::qtResourceDoubleClicked(const QModelIndex& index)
{
  Q_UNUSED(index)
}

void ctkPluginBrowser::dbResourceDoubleClicked(const QModelIndex& index)
{
  QString resPath = index.data(Qt::UserRole).toString();
  if (resPath.isEmpty() || resPath.endsWith('/')) return;

  qDebug() << "Trying to open: " << resPath;

  QModelIndex pluginIndex = ui.pluginsTableView->selectionModel()->selectedIndexes().first();
  long pluginId = pluginIndex.data(Qt::UserRole).toLongLong();

  QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->getPlugin(pluginId);

  QByteArray resContent = plugin->getResource(resPath);
  QString location = QString("/") + plugin->getSymbolicName() + resPath;
  editors->openEditor(location, resContent, location + " [cached]");
}

void ctkPluginBrowser::frameworkEvent(const ctkPluginFrameworkEvent& event)
{
  qDebug() << "FrameworkEvent: [" << event.getPlugin()->getSymbolicName() << "]" << event.getErrorString();
}

void ctkPluginBrowser::pluginEvent(const ctkPluginEvent& event)
{
  qDebug() << "PluginEvent: [" << event.getPlugin()->getSymbolicName() << "]" << pluginEventTypeToString[event.getType()];

  QSharedPointer<ctkPlugin> plugin = event.getPlugin();
  QModelIndexList selection = ui.pluginsTableView->selectionModel()->selectedIndexes();
  if (!selection.isEmpty() && selection.first().data(Qt::UserRole).toLongLong() == plugin->getPluginId())
  {
    updatePluginToolbar(plugin);
  }
}

void ctkPluginBrowser::serviceEvent(const ctkServiceEvent &event)
{
  qDebug() << "ServiceEvent: [" << event.getType() << "]" << event.getServiceReference().getUsingPlugins();
}

void ctkPluginBrowser::startPlugin()
{
  startPlugin(ctkPlugin::START_TRANSIENT | ctkPlugin::START_ACTIVATION_POLICY);
}

void ctkPluginBrowser::startPluginNow()
{
  startPlugin(ctkPlugin::START_TRANSIENT);
}

void ctkPluginBrowser::startPlugin(ctkPlugin::StartOptions options)
{
  QModelIndex selection = ui.pluginsTableView->selectionModel()->currentIndex();
  QVariant v = selection.data(Qt::UserRole);

  QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->getPlugin(v.toLongLong());
  plugin->start(options);
}

void ctkPluginBrowser::stopPlugin()
{
  QModelIndex selection = ui.pluginsTableView->selectionModel()->currentIndex();
  QVariant v = selection.data(Qt::UserRole);

  QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->getPlugin(v.toLongLong());
  plugin->stop();
}

void ctkPluginBrowser::closeEvent(QCloseEvent *closeEvent)
{
  QSettings settings;
  settings.setValue(SETTINGS_WND_GEOM, this->saveGeometry());
  settings.setValue(SETTINGS_WND_STATE, this->saveState());
  QMainWindow::closeEvent(closeEvent);
}
