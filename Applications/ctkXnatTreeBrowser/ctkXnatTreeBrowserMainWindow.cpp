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

#include "ctkXnatAssessor.h"
#include "ctkXnatAssessorFolder.h"
#include "ctkXnatDataModel.h"
#include "ctkXnatException.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatFile.h"
#include "ctkXnatLoginDialog.h"
#include "ctkXnatProject.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatResource.h"
#include "ctkXnatResourceFolder.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatSession.h"
#include "ctkXnatSubject.h"
#include "ctkXnatTreeModel.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

ctkXnatTreeBrowserMainWindow::ctkXnatTreeBrowserMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkXnatTreeBrowserMainWindow),
  m_Session(0),
  m_TreeModel(new ctkXnatTreeModel())
{
  ui->setupUi(this);

  ui->treeView->setModel(m_TreeModel);
  ui->downloadLabel->hide();

  this->connect(ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonPushed()));
  this->connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), SLOT(itemSelected(const QModelIndex&)));
  this->connect(ui->downloadButton, SIGNAL(clicked()), SLOT(downloadButtonClicked()));
  this->connect(ui->addResourceButton, SIGNAL(clicked()), SLOT(addResourceClicked()));
  this->connect(ui->uploadFileButton, SIGNAL(clicked()), SLOT(uploadFileClicked()));
}

ctkXnatTreeBrowserMainWindow::~ctkXnatTreeBrowserMainWindow()
{
  if (m_Session)
  {
    delete m_Session;
  }
  delete ui;

  delete m_TreeModel;
}

void ctkXnatTreeBrowserMainWindow::loginButtonPushed()
{
  if (m_Session)
  {
    ctkXnatDataModel* dataModel = m_Session->dataModel();
    m_TreeModel->removeDataModel(dataModel);
    ui->treeView->reset();
    delete m_Session;
    m_Session = 0;
    ui->loginButton->setText("Login");
    ui->loginLabel->setText("Disconnected");
    ui->downloadLabel->hide();
  }
  else
  {
    ctkXnatLoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted)
    {
      m_Session = loginDialog.session();
      if (m_Session)
      {
        ui->loginButton->setText("Logout");
        ui->loginLabel->setText(QString("Connected: %1").arg(m_Session->url().toString()));

        ctkXnatDataModel* dataModel = m_Session->dataModel();
        m_TreeModel->addDataModel(dataModel);
        ui->treeView->reset();
        ui->downloadLabel->show();
      }
    }
  }
}

void ctkXnatTreeBrowserMainWindow::itemSelected(const QModelIndex &index)
{
  ctkXnatObject* xnatObject = m_TreeModel->xnatObject(index);
  bool downloadable = false;
  downloadable |= dynamic_cast<ctkXnatFile*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatScan*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatScanFolder*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatAssessor*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatAssessorFolder*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatResource*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatReconstruction*>(xnatObject)!=NULL;
  downloadable |= dynamic_cast<ctkXnatReconstructionFolder*>(xnatObject)!=NULL;
  ui->downloadButton->setEnabled(downloadable);
  ui->downloadLabel->setVisible(!downloadable);
  bool canHaveResource = false;
  canHaveResource |= dynamic_cast<ctkXnatProject*>(xnatObject) != NULL;
  canHaveResource |=  dynamic_cast<ctkXnatSubject*>(xnatObject) != NULL;
  canHaveResource |=  dynamic_cast<ctkXnatExperiment*>(xnatObject) != NULL;
  ui->addResourceButton->setEnabled(canHaveResource);
  bool uploadFilePossible = false;
  uploadFilePossible |= dynamic_cast<ctkXnatResource*>(xnatObject) != NULL;
  uploadFilePossible |=  dynamic_cast<ctkXnatScan*>(xnatObject) != NULL;
  uploadFilePossible |=  dynamic_cast<ctkXnatAssessor*>(xnatObject) != NULL;
  ui->uploadFileButton->setEnabled(uploadFilePossible);
}

void ctkXnatTreeBrowserMainWindow::downloadButtonClicked()
{
  const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
  QVariant variant = m_TreeModel->data(index, Qt::DisplayRole);
  QString fileName = variant.value<QString>();
  if ( fileName.length() != 0 )
  {
    m_TreeModel->downloadFile(index, fileName);
  }
}

void ctkXnatTreeBrowserMainWindow::addResourceClicked()
{
  const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
  ctkXnatObject* parentObject = m_TreeModel->xnatObject(index);
  parentObject->addResourceFolder("data", "ctk format", "custom content", "tag1, tag2");
}

void ctkXnatTreeBrowserMainWindow::uploadFileClicked()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());
  const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
  ctkXnatResource* resource = dynamic_cast<ctkXnatResource*>(m_TreeModel->xnatObject(index));
  if (resource)
  {
    ctkXnatFile* file = new ctkXnatFile();
    file->setLocalFilePath(filename);
    QFileInfo fileInfo (filename);
    file->setName(fileInfo.fileName());
    file->setFileFormat("some format");
    file->setFileContent("some content");
    file->setFileTags("some, tags");
    resource->add(file);
    try
    {
      file->save();
    }
    catch (ctkXnatException &e)
    {
      QMessageBox msgbox;
      msgbox.setText(e.what());
      msgbox.setIcon(QMessageBox::Critical);
      msgbox.exec();
    }
    m_TreeModel->addChildNode(index, file);
  }
}
