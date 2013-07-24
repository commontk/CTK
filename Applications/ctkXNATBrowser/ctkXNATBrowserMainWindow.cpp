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
#include "ctkXnatConnectionFactory.h"
#include "ctkXnatServer.h"
#include "ctkXnatProject.h"

#include <QDebug>

ctkXNATBrowserMainWindow::ctkXNATBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXNATBrowserMainWindow),
  m_ConnectionFactory(new ctkXnatConnectionFactory()),
  m_Connection(0),
  m_ProjectsModel(new ctkXnatProjectListModel()),
  m_SubjectsModel(new ctkXnatProjectListModel())
{
  ui->setupUi(this);

  ui->projectsList->setModel(m_ProjectsModel);
  ui->subjectsList->setModel(m_SubjectsModel);

  this->connect(ui->projectsList, SIGNAL(clicked(QModelIndex)), SLOT(projectSelected(QModelIndex)));
  this->connect(ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonPushed()));
}

ctkXNATBrowserMainWindow::~ctkXNATBrowserMainWindow()
{
  if (m_Connection)
  {
    delete m_Connection;
  }
  delete m_ConnectionFactory;
  delete ui;

  delete m_SubjectsModel;
  delete m_ProjectsModel;
}

void ctkXNATBrowserMainWindow::loginButtonPushed()
{
  if (m_Connection)
  {
    delete m_Connection;
    m_Connection = 0;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");

    m_ProjectsModel->setRootObject(ctkXnatObject::Pointer());
    ui->projectsList->reset();
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
        //xnatConnection->fetch(server);
        server->fetch();
        m_ProjectsModel->setRootObject(server);
        ui->projectsList->reset();
      }
    }
  }
}

void ctkXNATBrowserMainWindow::projectSelected(const QModelIndex& index)
{
  qDebug() << "Project selected";
  QVariant variant = m_ProjectsModel->data(index, Qt::UserRole);
  if (variant.isValid())
  {
    ctkXnatObject::Pointer project = variant.value<ctkXnatObject::Pointer>();
    qDebug() << "selected project id:" << project->getId();
    project->fetch();
    m_SubjectsModel->setRootObject(project);
    ui->subjectsList->reset();
  }
}
