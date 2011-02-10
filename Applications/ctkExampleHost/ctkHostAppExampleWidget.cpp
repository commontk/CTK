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
#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppService.h"
#include <ctkDicomAppHostingTypesHelper.h>

//----------------------------------------------------------------------------
ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
  qDebug() << "setup ui";
  ui->setupUi(this);
  ui->crashLabel->setVisible(false);
  ui->messageOutput->setVisible(false);
  this->Host = new ctkExampleDicomHost(ui->placeholderFrame);

  connect(&this->Host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
  connect(&this->Host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));
  connect(ui->placeholderFrame,SIGNAL(resized()),SLOT(placeholderResized()));
  connect(this->Host,SIGNAL( stateChangedReceived(ctkDicomAppHosting::State)),SLOT(appStateChanged(ctkDicomAppHosting::State)));

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
void ctkHostAppExampleWidget::startButtonClicked()
{
  qDebug() << "start button clicked";
  if (this->Host)
    {
    this->Host->StartApplication(this->AppFileName);
    //forward output to textedit
    connect(&this->Host->getAppProcess(),SIGNAL(readyReadStandardOutput()),this,SLOT(outputMessage()));
    }
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::runButtonClicked()
{
  qDebug() << "run button clicked";
  if (this->Host)
    {
    bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    qDebug() << "  setState(INPROGRESS) returned: " << reply;
    }
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::stopButtonClicked()
{
  qDebug() << "stop button clicked";
  this->Host->getDicomAppService ()->setState (ctkDicomAppHosting::CANCELED);
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
  this->AppFileName = name;
  if (QFile(this->AppFileName).permissions() & QFile::ExeUser )
    {
    this->ui->applicationPathLabel->setText(this->AppFileName);
    }
  else
    {
    this->ui->applicationPathLabel->setText(
        QString("<font color='red'>Not executable:</font>").append(this->AppFileName));
    }
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

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::appProcessStateChanged(QProcess::ProcessState state)
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

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::placeholderResized()
{
  qDebug() << "resized";
  //ui->placeholderFrame->printPosition();
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::appStateChanged(ctkDicomAppHosting::State state)
{
  ui->statusLabel->setText(ctkDicomSoapState::toStringValue(state));

  bool reply;
  ctkDicomAppHosting::ObjectDescriptor ourObjectDescriptor;
  QList<ctkDicomAppHosting::Study> studies;
  ctkDicomAppHosting::AvailableData data;
  ctkDicomAppHosting::Patient patient;

  //TODO put the state changed routine back in notifyStateChanged for the state machine part.
  switch (state)
  {
  case ctkDicomAppHosting::IDLE:
    if (this->Host->getApplicationState() != ctkDicomAppHosting::IDLE)
    {
      qDebug()<<"state was not IDLE before -> setState EXIT ";
      this->Host->getDicomAppService()->setState(ctkDicomAppHosting::EXIT);
    }
    break;
  case ctkDicomAppHosting::INPROGRESS:
    patient.name = "John Doe";
    patient.id = "0000";
    patient.assigningAuthority = "authority";
    patient.sex = "male";
    patient.birthDate = "today";
    patient.objectDescriptors = QList<ctkDicomAppHosting::ObjectDescriptor>();

    patient.studies = studies;

    ourObjectDescriptor.descriptorUUID = QUuid("{11111111-1111-1111-1111-111111111111}");
    ourObjectDescriptor.mimeType = "text/plain";
    ourObjectDescriptor.classUID = "lovelyClass";
    ourObjectDescriptor.transferSyntaxUID = "transSyntaxUId";
    ourObjectDescriptor.modality = "modMod";

    data.objectDescriptors =  QList<ctkDicomAppHosting::ObjectDescriptor>();
    data.objectDescriptors.append (ourObjectDescriptor);
    data.patients = QList<ctkDicomAppHosting::Patient>();
    data.patients.append (patient);

    qDebug()<<"send dataDescriptors";
    reply = this->Host->getDicomAppService()->notifyDataAvailable (data,true);
    qDebug() << "  notifyDataAvailable(1111) returned: " << reply;
    break;
  case ctkDicomAppHosting::COMPLETED:
  case ctkDicomAppHosting::SUSPENDED:
  case ctkDicomAppHosting::CANCELED:
  case ctkDicomAppHosting::EXIT:
    //shouldn't happen, when exiting the application just dies
  default:
    //do nothing
    break;
  }
  this->Host->setApplicationState(state);
}

//----------------------------------------------------------------------------
void ctkHostAppExampleWidget::outputMessage ()
{
  ui->messageOutput->append (this->Host->processReadAll ());
}
