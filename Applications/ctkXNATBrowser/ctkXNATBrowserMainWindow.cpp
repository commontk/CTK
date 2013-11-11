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
#include "ctkXnatSession.h"
#include "ctkXnatSessionFactory.h"
#include "ctkXnatDataModel.h"
#include "ctkXnatProject.h"

#include <QDebug>

ctkXNATBrowserMainWindow::ctkXNATBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXNATBrowserMainWindow),
  m_SessionFactory(new ctkXnatSessionFactory()),
  m_Session(0),
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
  if (m_Session)
  {
    delete m_Session;
  }
  delete m_SessionFactory;
  delete ui;

  delete m_SubjectsModel;
  delete m_ProjectsModel;
}

void ctkXNATBrowserMainWindow::loginButtonPushed()
{
  if (m_Session)
  {
    delete m_Session;
    m_Session = 0;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");

    m_ProjectsModel->setRootObject(0);
    ui->projectsList->reset();
  }
  else
  {
    ctkXnatLoginDialog loginDialog(m_SessionFactory);
    if (loginDialog.exec() == QDialog::Accepted)
    {
      m_Session = loginDialog.getSession();
      if (m_Session)
      {
        ui->loginButton->setText("Logout");
        ui->loginLabel->setText(QString("Connected: %1").arg(m_Session->url()));

        ctkXnatDataModel* dataModel = m_Session->dataModel();
        //xnatConnection->fetch(server);
        dataModel->fetch();
        m_ProjectsModel->setRootObject(dataModel);
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
    ctkXnatObject* project = variant.value<ctkXnatObject*>();
    qDebug() << "selected project id:" << project->id();
    project->fetch();
    m_SubjectsModel->setRootObject(project);
    ui->subjectsList->reset();
  }
}
