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

#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppService.h"
#include <ctkDicomAppHostingTypesHelper.h>

#include <QDebug>
#include <QFileDialog>
#include <QProcess>

ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
  qDebug() << "setup ui";
  ui->setupUi(this);
  ui->crashLabel->setVisible(false);
  ui->messageOutput->setVisible(false);
  this->host = new ctkExampleDicomHost(ui->placeholderFrame);

  connect(&this->host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
  connect(&this->host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));
  connect(ui->placeholderFrame,SIGNAL(resized()),SLOT(placeholderResized()));
  connect(this->host,SIGNAL( stateChangeReceived(ctkDicomAppHosting::State)),SLOT(appStateChanged(ctkDicomAppHosting::State)));

}

ctkHostAppExampleWidget::~ctkHostAppExampleWidget()
{
  delete host;
  delete ui;
}

void ctkHostAppExampleWidget::startButtonClicked()
{
  qDebug() << "start button clicked";
  if (host)
  {
    host->StartApplication(appFileName);
  }
}

void ctkHostAppExampleWidget::runButtonClicked()
{
  qDebug() << "run button clicked";
  if (host)
  {
    bool reply = host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    qDebug() << "  setState(INPROGRESS) returned: " << reply;
  }
}

void ctkHostAppExampleWidget::stopButtonClicked()
{
  qDebug() << "stop button clicked";
}

void ctkHostAppExampleWidget::loadButtonClicked()
{
  qDebug() << "load button clicked";
  this->setAppFileName(QFileDialog::getOpenFileName(this,"Choose hosted application",QApplication::applicationDirPath()));
}

void ctkHostAppExampleWidget::setAppFileName(QString name)
{
  this->appFileName = name;
  if (QFile(this->appFileName).permissions() & QFile::ExeUser )
  {
    this->ui->applicationPathLabel->setText(this->appFileName);
  }
  else
  {
    this->ui->applicationPathLabel->setText(QString("<font color='red'>Not executable:</font>").append(this->appFileName));
  }
}

void ctkHostAppExampleWidget::appProcessError(QProcess::ProcessError error)
{
  if (error == QProcess::Crashed)
  {
    qDebug() << "crash detected";
    ui->crashLabel->setVisible(true);
  }
}

void ctkHostAppExampleWidget::appProcessStateChanged(QProcess::ProcessState state)
{
  QString labelText;
  switch (state){
  case QProcess::Running:
    ui->processStateLabel->setText("Running");
    break;
  case QProcess::NotRunning:
    if (host->getAppProcess().exitStatus() == QProcess::CrashExit )
    {
      labelText = "crashed";
    }
    else
    {
      labelText = "Not Running, last exit code ";
      labelText.append(QString::number(host->getAppProcess().exitCode()));
    }
    ui->processStateLabel->setText(labelText);
    break;
  case QProcess::Starting:
    ui->processStateLabel->setText("Starting");
    break;
  default:
    ;
  }
}

void ctkHostAppExampleWidget::placeholderResized()
{
  qDebug() << "resized";
  //ui->placeholderFrame->printPosition();
}

void ctkHostAppExampleWidget::appStateChanged(ctkDicomAppHosting::State state)
{
  ui->statusLabel->setText(ctkDicomSoapState::toStringValue(state));
}
