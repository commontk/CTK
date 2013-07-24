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
#include "ctkXnatConnectionFactory.h"
#include "ctkXnatServer.h"
#include "ctkXnatProject.h"

ctkXnatTreeBrowserMainWindow::ctkXnatTreeBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXnatTreeBrowserMainWindow),
  m_ConnectionFactory(new ctkXnatConnectionFactory()),
  m_Connection(0),
  m_TreeModel(new ctkXnatTreeModel())
{
  ui->setupUi(this);

  ui->treeView->setModel(m_TreeModel);

  this->connect(ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonPushed()));
}

ctkXnatTreeBrowserMainWindow::~ctkXnatTreeBrowserMainWindow()
{
  if (m_Connection)
  {
    delete m_Connection;
  }
  delete m_ConnectionFactory;
  delete ui;

  delete m_TreeModel;
}

void ctkXnatTreeBrowserMainWindow::loginButtonPushed()
{
  if (m_Connection)
  {
    delete m_Connection;
    m_Connection = 0;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");
  }
  else
  {
    ctkXnatLoginDialog loginDialog(m_ConnectionFactory);
    if (loginDialog.exec() == QDialog::Accepted)
    {
      m_Connection = loginDialog.getConnection();
      if (m_Connection)
      {
        ui->loginButton->setText("Logout");
        ui->loginLabel->setText(QString("Connected: %1").arg(m_Connection->url()));

        ctkXnatServer::Pointer server = m_Connection->server();
        m_TreeModel->addServer(server);
        ui->treeView->reset();
      }
    }
  }
}
