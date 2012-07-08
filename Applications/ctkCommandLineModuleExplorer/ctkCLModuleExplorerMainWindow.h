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

#ifndef CTKCLIPLUGINEXPLORERMAINWINDOW_H
#define CTKCLIPLUGINEXPLORERMAINWINDOW_H

#include <QMainWindow>

#include <ctkModuleManager.h>

class ctkModuleDescriptionDefaultFactory;

namespace Ui {
class ctkCLModuleExplorerMainWindow;
}

class ctkCLModuleExplorerMainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit ctkCLModuleExplorerMainWindow(QWidget *parent = 0);
  ~ctkCLModuleExplorerMainWindow();

  void addModule(const QString& location);
  void testModuleXML(const QByteArray& xml);

protected Q_SLOTS:

  void on_actionRun_triggered();

  void futureFinished();

protected:

  void addModuleTab(const ctkModuleReference& moduleRef);

  ctkModuleReference moduleReference(int tabIndex);
  
private:
  Ui::ctkCLModuleExplorerMainWindow *ui;

  ctkModuleDescriptionDefaultFactory* factory;
  ctkModuleManager moduleManager;

  QHash<int, ctkModuleReference> mapTabToModuleRef;

  ctkModuleProcessFutureWatcher futureWatcher;
};

#endif // CTKCLIPLUGINEXPLORERMAINWINDOW_H
