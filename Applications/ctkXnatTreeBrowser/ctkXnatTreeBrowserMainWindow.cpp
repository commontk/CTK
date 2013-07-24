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

#include "ctkXnatTreeBrowserMainWindow.h"
#include "ui_ctkXnatTreeBrowserMainWindow.h"

#include "ctkXnatLoginDialog.h"
#include "ctkXnatTreeModel.h"
#include "ctkXnatConnection.h"
#include "ctkXnatServer.h"
#include "ctkXnatProject.h"

#include <QDebug>

ctkXnatTreeBrowserMainWindow::ctkXnatTreeBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXnatTreeBrowserMainWindow),
  xnatConnection(0),
  treeModel(new ctkXnatTreeModel())
{
  ui->setupUi(this);

  ui->treeView->setModel(treeModel);

  connect(ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonPushed()));
}

ctkXnatTreeBrowserMainWindow::~ctkXnatTreeBrowserMainWindow()
{
  delete treeModel;
  delete ui;
}

void ctkXnatTreeBrowserMainWindow::loginButtonPushed()
{
  if (xnatConnection)
  {
    delete xnatConnection;
    xnatConnection = NULL;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");
  }
  else
  {
    ctkXnatLoginDialog loginDialog(xnatConnectionFactory);
    if (loginDialog.exec() == QDialog::Accepted)
    {
      xnatConnection = loginDialog.getConnection();
      if (xnatConnection)
      {
        ui->loginButton->setText("Logout");
        ui->loginLabel->setText(QString("Connected: %1").arg(xnatConnection->url()));

        ctkXnatServer::Pointer server = xnatConnection->server();
        treeModel->addServer(server);
        ui->treeView->reset();
      }
    }
  }
}
