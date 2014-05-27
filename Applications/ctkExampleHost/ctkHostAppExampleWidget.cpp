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

// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QVBoxLayout>

// CTK includes
#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppService.h"
#include <ctkExampleHostControlWidget.h>
#include <ctkDicomAppHostingTypesHelper.h>

//----------------------------------------------------------------------------
ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
  qDebug() << "setup ui";
  ui->setupUi(this);
  ui->crashLabel->setVisible(false);
  this->Host = new ctkExampleDicomHost(ui->placeholderFrame);
  this->HostControls = new ctkExampleHostControlWidget(Host, ui->placeHolderForControls);
  this->HostControls->setObjectName(QString::fromUtf8("exampleHostControls"));
  ui->verticalLayout->addWidget(HostControls);

  connect(&this->Host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
  connect(ui->placeholderFrame,SIGNAL(resized()),SLOT(placeholderResized()));
}

//----------------------------------------------------------------------------
ctkHostAppExampleWidget::~ctkHostAppExampleWidget()
{
  delete this->Host;
  this->Host = 0;
  delete this->ui;
  this->ui = 0;
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::loadButtonClicked()
{
  qDebug() << "load button clicked";
  this->setAppFileName(QFileDialog::getOpenFileName(this,"Choose hosted application",QApplication::applicationDirPath()));
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::setAppFileName(QString name)
{
  HostControls->setAppFileName(name);
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::placeholderResized()
{
  qDebug() << "resized";
  //ui->placeholderFrame->printPosition();
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::appProcessError(QProcess::ProcessError error)
{
  if (error == QProcess::Crashed)
    {
    qDebug() << "crash detected";
    ui->crashLabel->setVisible(true);
    }
}
