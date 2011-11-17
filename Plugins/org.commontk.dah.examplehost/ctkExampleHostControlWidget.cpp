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

// CTK includes
#include "ctkExampleHostControlWidget.h"
#include "ui_ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppService.h"
#include <ctkDicomAppHostingTypesHelper.h>

//----------------------------------------------------------------------------
ctkExampleHostControlWidget::ctkExampleHostControlWidget(ctkExampleDicomHost * host, QWidget *parent) :
    Host(host),
    QWidget(parent),
    ValidAppFileName(false),
    ui(new Ui::ctkExampleHostControlWidget)    
{
  qDebug() << "setup ui";
  ui->setupUi(this);
//  ui->crashLabel->setVisible(false);
  ui->messageOutput->setVisible(true);
  if(Host==NULL)
    qDebug() << "no host provided when creating ctkExampleHostControlWidget.";
  else
  {
    connect(&this->Host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
    connect(&this->Host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));
    connect(this->Host,SIGNAL(stateChangedReceived(ctkDicomAppHosting::State)),SLOT(appStateChanged(ctkDicomAppHosting::State)));
  }
}

//----------------------------------------------------------------------------
ctkExampleHostControlWidget::~ctkExampleHostControlWidget()
{
  delete this->ui;
  this->ui = 0;
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::StartApplication(QString appFileName)
{
  qDebug() << "ctkExampleHostControlWidget::StartApplication";
  if(appFileName.isEmpty()==false)
    this->setAppFileName(appFileName);
  if ((this->Host) && (validAppFileName()))
    {
    qDebug() << "Starting app";
    this->Host->StartApplication(this->AppFileName);
    //forward output to textedit
    connect(&this->Host->getAppProcess(),SIGNAL(readyReadStandardOutput()),this,SLOT(outputMessage()));
    }
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::runButtonClicked()
{
  qDebug() << "run button clicked";
  if (this->Host)
    {
    bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    qDebug() << "  setState(INPROGRESS) returned: " << reply;
    }
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::stopButtonClicked()
{
  qDebug() << "stop button clicked";
  this->Host->exitApplication();
}


//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::setAppFileName(QString name)
{
  this->AppFileName = name;
  if (QFile(this->AppFileName).permissions() & QFile::ExeUser )
    {
    this->ui->applicationPathLabel->setText(this->AppFileName);
    ValidAppFileName = true;
    }
  else
    {
    ValidAppFileName = false;
    this->ui->applicationPathLabel->setText(
        QString("<font color='red'>Not executable:</font>").append(this->AppFileName));
    }
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::appProcessError(QProcess::ProcessError error)
{
  if (error == QProcess::Crashed)
    {
    qDebug() << "crash detected";
//    ui->crashLabel->setVisible(true);
    }
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::appProcessStateChanged(QProcess::ProcessState state)
{
  QString labelText;
  switch (state)
    {
    case QProcess::Running:
      ui->processStateLabel->setText("Running");
      break;
    case QProcess::NotRunning:
      if (this->Host->getAppProcess().exitStatus() == QProcess::CrashExit )
      {
        labelText = "crashed";
      }
      else
      {
        labelText = "Not Running, last exit code ";
        labelText.append(QString::number(this->Host->getAppProcess().exitCode()));
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


void ctkExampleHostControlWidget::appStateChanged(ctkDicomAppHosting::State state)
{
  ui->statusLabel->setText(ctkDicomSoapState::toStringValue(state));
}


//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::outputMessage ()
{
  ui->messageOutput->append (this->Host->processReadAll ());
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::suspendButtonClicked()
{
  this->Host->getDicomAppService()->setState(ctkDicomAppHosting::SUSPENDED);
}

//----------------------------------------------------------------------------
void ctkExampleHostControlWidget::cancelButtonClicked()
{
  this->Host->getDicomAppService()->setState(ctkDicomAppHosting::CANCELED);
}

//----------------------------------------------------------------------------
bool ctkExampleHostControlWidget::validAppFileName()
{
  return ValidAppFileName;
}
