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

#include "ctkXNATBrowserMainWindow.h"
#include "ui_ctkXNATBrowserMainWindow.h"

#include "ctkXnatLoginDialog.h"
#include "ctkXnatProjectListModel.h"
#include "ctkXnatConnection.h"
#include "ctkXnatServer.h"
#include "ctkXnatProject.h"

#include <QDebug>

ctkXNATBrowserMainWindow::ctkXNATBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXNATBrowserMainWindow),
  xnatConnection(NULL),
  projectsModel(new ctkXnatProjectListModel()),
  subjectsModel(new ctkXnatProjectListModel())
{
  ui->setupUi(this);

  ui->projectsList->setModel(projectsModel);
  ui->subjectsList->setModel(subjectsModel);

  connect(ui->projectsList, SIGNAL(clicked(QModelIndex)), SLOT(projectSelected(QModelIndex)));
  connect(ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonPushed()));
}

ctkXNATBrowserMainWindow::~ctkXNATBrowserMainWindow()
{
  delete ui;
}

void ctkXNATBrowserMainWindow::loginButtonPushed()
{
  if (xnatConnection)
  {
    delete xnatConnection;
    xnatConnection = NULL;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");

    projectsModel->setRootObject(ctkXnatObject::Pointer());
    ui->projectsList->reset();
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
        //xnatConnection->fetch(server);
        server->fetch();
        projectsModel->setRootObject(server);
        ui->projectsList->reset();
      }
    }
  }
}

void ctkXNATBrowserMainWindow::projectSelected(const QModelIndex& index)
{
  qDebug() << "Project selected";
  QVariant variant = projectsModel->data(index, Qt::UserRole);
  if (variant.isValid())
  {
    ctkXnatObject::Pointer project = variant.value<ctkXnatObject::Pointer>();
    qDebug() << "selected project id:" << project->getId();
    project->fetch();
    subjectsModel->setRootObject(project);
    ui->subjectsList->reset();
  }
}
