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

#ifndef CTKPLUGINBROWSER_H
#define CTKPLUGINBROWSER_H

#include <QMainWindow>

#include <ui_ctkPluginBrowserMainWindow.h>

#include "ctkPluginBrowserEditors.h"

#include <ctkPluginEvent.h>
#include <ctkPluginFrameworkEvent.h>
#include <ctkPlugin.h>
#include <ctkServiceEvent.h>


class ctkPluginFramework;

class ctkPluginBrowser : public QMainWindow
{
  Q_OBJECT

public:

  ctkPluginBrowser(ctkPluginFramework* framework);

private Q_SLOTS:

  void pluginSelected(const QModelIndex& index);
  void pluginDoubleClicked(const QModelIndex& index);
  void qtResourceDoubleClicked(const QModelIndex& index);
  void dbResourceDoubleClicked(const QModelIndex& index);

  void frameworkEvent(const ctkPluginFrameworkEvent& event);
  void pluginEvent(const ctkPluginEvent& event);
  void serviceEvent(const ctkServiceEvent& event);

  void startPlugin();
  void startPluginNow();
  void stopPlugin();

private:

  void closeEvent(QCloseEvent* closeEvent);

  void updatePluginToolbar(QSharedPointer<ctkPlugin> plugin);
  void startPlugin(ctkPlugin::StartOptions options);

  QMap<ctkPluginEvent::Type, QString> pluginEventTypeToString;

  ctkPluginFramework* framework;

  Ui::ctkPluginBrowserWindow ui;
  ctkPluginBrowserEditors* editors;

  QAction* startPluginNowAction;
  QAction* startPluginAction;
  QAction* stopPluginAction;
};

#endif // CTKPLUGINBROWSER_H
