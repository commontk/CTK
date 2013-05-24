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

#ifndef CTKXNATBROWSERMAINWINDOW_H
#define CTKXNATBROWSERMAINWINDOW_H

#include <QMainWindow>

class QModelIndex;

#include "ctkXnatConnectionFactory.h"

class ctkXnatConnection;
class ctkXnatProjectListModel;
class ctkXnatProject;

namespace Ui {
class ctkXNATBrowserMainWindow;
}

class ctkXNATBrowserMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit ctkXNATBrowserMainWindow(QWidget *parent = 0);
  ~ctkXNATBrowserMainWindow();

private Q_SLOTS:

  void loginButtonPushed();

  void projectSelected(const QModelIndex& index);

private:
  Ui::ctkXNATBrowserMainWindow *ui;

  ctkXnatConnection* xnatConnection;
  ctkXnatProjectListModel* projectsModel;
  ctkXnatProjectListModel* subjectsModel;
  ctkXnatConnectionFactory xnatConnectionFactory;
};

#endif // CTKXNATBROWSERMAINWINDOW_H
