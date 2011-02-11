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

#include <ctkDICOMDataset.h>
// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>

void addToAvailableData(ctkDicomAppHosting::AvailableData& data, 
                        QHash<QString, ctkDicomAppHosting::ObjectLocator>& locatorHash, 
                        const ctkDICOMDataset& dataset, 
                        long length, 
                        long offset, 
                        const QString& uri)
{
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor;
  ctkDicomAppHosting::Study study;
  ctkDicomAppHosting::Series series;
  ctkDicomAppHosting::Patient patient;
  patient.name = dataset.GetElementAsString(DCM_PatientsName);
qDebug()<<"Patient:  " << patient.name;
  patient.id = dataset.GetElementAsString(DCM_PatientID);
  patient.assigningAuthority = dataset.GetElementAsString(DCM_IssuerOfPatientID);
  patient.sex = dataset.GetElementAsString(DCM_PatientsSex);
  patient.birthDate = dataset.GetElementAsString(DCM_PatientsBirthDate);

  study.studyUID = dataset.GetElementAsString(DCM_StudyInstanceUID);
  series.seriesUID = dataset.GetElementAsString(DCM_SeriesInstanceUID);

  QString uuid = QUuid::createUuid().toString();
  objectDescriptor.descriptorUUID = uuid;
  objectDescriptor.mimeType = "application/dicom";
  objectDescriptor.classUID = dataset.GetElementAsString(DCM_SOPClassUID);
  objectDescriptor.transferSyntaxUID = dataset.GetElementAsString(DCM_TransferSyntaxUID);
  objectDescriptor.modality = dataset.GetElementAsString(DCM_Modality);
  
  series.objectDescriptors = QList<ctkDicomAppHosting::ObjectDescriptor>();
  series.objectDescriptors.append (objectDescriptor);

  study.series.append( series);

  patient.studies.append(study);

  data.patients = QList<ctkDicomAppHosting::Patient>();
  data.patients.append(patient);

  ctkDicomAppHosting::ObjectLocator locator;
  locator.locator = objectDescriptor.descriptorUUID;
  locator.source = objectDescriptor.descriptorUUID;
  locator.offset = offset;
  locator.length = length;
  locator.transferSyntax = objectDescriptor.transferSyntaxUID;
  locator.URI = uri;
}

void addToAvailableData(ctkDicomAppHosting::AvailableData& data, QHash<QString, ctkDicomAppHosting::ObjectLocator>& locatorHash, const QString& filename)
{
  QFileInfo fileinfo(filename);
  qDebug() << filename << " " << fileinfo.exists();

  ctkDICOMDataset ctkdataset;
  ctkdataset.InitializeFromFile(filename, EXS_Unknown, EGL_noChange, 400);

  QString uri("file:/");
  uri.append(fileinfo.absoluteFilePath());
  addToAvailableData(data, locatorHash, ctkdataset, fileinfo.size(), 0, uri);
}

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
  QHash<QString, ctkDicomAppHosting::ObjectLocator> uuidhash;

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

    addToAvailableData(data, uuidhash, "C:/XIP/XIPHost/dicom-dataset-demo/1.3.6.1.4.1.9328.50.1.10698.dcm");
    //patient.name = "John Doe";
    //patient.id = "0000";
    //patient.assigningAuthority = "authority";
    //patient.sex = "male";
    //patient.birthDate = "today";
    //patient.objectDescriptors = QList<ctkDicomAppHosting::ObjectDescriptor>();

    //patient.studies = studies;

    //ourObjectDescriptor.descriptorUUID = QUuid("{11111111-1111-1111-1111-111111111111}");
    //ourObjectDescriptor.mimeType = "text/plain";
    //ourObjectDescriptor.classUID = "lovelyClass";
    //ourObjectDescriptor.transferSyntaxUID = "transSyntaxUId";
    //ourObjectDescriptor.modality = "modMod";

    //data.objectDescriptors =  QList<ctkDicomAppHosting::ObjectDescriptor>();
    //data.objectDescriptors.append (ourObjectDescriptor);
    //data.patients = QList<ctkDicomAppHosting::Patient>();
    //data.patients.append (patient);

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
