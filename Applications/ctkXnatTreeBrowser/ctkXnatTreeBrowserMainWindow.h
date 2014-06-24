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

#ifndef CTKXNATTREEBROWSERMAINWINDOW_H
#define CTKXNATTREEBROWSERMAINWINDOW_H

#include <QMainWindow>

class QModelIndex;

class ctkXnatSession;
class ctkXnatTreeModel;

namespace Ui {
class ctkXnatTreeBrowserMainWindow;
}

class ctkXnatTreeBrowserMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit ctkXnatTreeBrowserMainWindow(QWidget *parent = 0);
  ~ctkXnatTreeBrowserMainWindow();

private Q_SLOTS:

  void loginButtonPushed();
  void itemSelected(const QModelIndex&);
  void downloadButtonClicked();

private:
  Ui::ctkXnatTreeBrowserMainWindow* ui;

  ctkXnatSession* m_Session;
  ctkXnatTreeModel* m_TreeModel;
};

#endif
