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
#include <QProcess>
#include <QtDebug>
#include <QRect>

// CTK includes
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppHostingTypesHelper.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
ctkExampleDicomHost::ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort, int appPort) :
    ctkDicomAbstractHost(hostPort, appPort),
    PlaceholderWidget(placeholderWidget),
    exitingApplication(false)
{
  connect(this,SIGNAL(appReady()),SLOT(onAppReady()));
  connect(this,SIGNAL(startProgress()),this,SLOT(onStartProgress()));
  connect(this,SIGNAL(releaseAvailableResources()),this,SLOT(onReleaseAvailableResources()));
  connect(this,SIGNAL(resumed()),this,SLOT(onResumed()));
  connect(this,SIGNAL(completed()),this,SLOT(onCompleted()));
  connect(this,SIGNAL(suspended()),this,SLOT(onSuspended()));
  connect(this,SIGNAL(canceled()),this,SLOT(onCanceled()));
  connect(this,SIGNAL(exited()),this,SLOT(onExited()));
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::StartApplication(QString AppPath)
{
  QStringList arguments;
  arguments.append("--hostURL");
  arguments.append(QString("http://localhost:") + QString::number(this->getHostPort()) + "/HostInterface" );
  arguments.append("--applicationURL");
  arguments.append(QString("http://localhost:") + QString::number(this->getAppPort()) + "/ApplicationInterface" );
  //by default, the ctkExampleHostedApp uses the org.commontk.dah.exampleapp plugin
  //arguments.append("dicomapp"); // the app plugin to use - has to be changed later
  //if (!QProcess::startDetached (
  //{
  //    qCritical() << "application failed to start!";
  //}
  //qDebug() << "starting application: " << AppPath << " " << arguments;
  qDebug() << "starting application: " << AppPath << " " << arguments;
  this->AppProcess.setProcessChannelMode(QProcess::MergedChannels);
  this->AppProcess.start(AppPath, arguments);
}

//----------------------------------------------------------------------------
QRect ctkExampleDicomHost::getAvailableScreen(const QRect& preferredScreen)
{
  qDebug()<< "Application asked for this area:"<< preferredScreen;

  QRect rect (this->PlaceholderWidget->getAbsolutePosition());

  emit giveAvailableScreen(rect);
  return rect;
}


//----------------------------------------------------------------------------
void ctkExampleDicomHost::notifyStatus(const ctkDicomAppHosting::Status& status)
{
  qDebug()<< "new status received:"<<status.codeMeaning;
  emit statusReceived(status);;
}

//----------------------------------------------------------------------------
ctkExampleDicomHost::~ctkExampleDicomHost()
{
  qDebug() << "Exiting host: trying to terminate app";
  this->AppProcess.terminate();
  qDebug() << "Exiting host: trying to kill app";
  this->AppProcess.kill();
}


//----------------------------------------------------------------------------
bool ctkExampleDicomHost::notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData)
{
  Q_UNUSED(data)
  Q_UNUSED(lastData)
  return false;
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onAppReady()
{
  //prepare some resources...
  //tell app to start
  //getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
  qDebug() << "App ready to work";
  if (this->exitingApplication)
  {
    this->exitingApplication = false;
    getDicomAppService ()->setState (ctkDicomAppHosting::EXIT);
  }
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onStartProgress()
{
  ctkDicomAppHosting::ObjectDescriptor ourObjectDescriptor;
  QList<ctkDicomAppHosting::Study> studies;
  ctkDicomAppHosting::AvailableData data;
  ctkDicomAppHosting::Patient patient;

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
  bool reply = getDicomAppService()->notifyDataAvailable (data,true);
  qDebug() << "  notifyDataAvailable(1111) returned: " << reply;
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onResumed()
{
  qDebug() << "App resumed work";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onCompleted()
{
  qDebug() << "App finished processing";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onSuspended()
{
  qDebug() << "App paused";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onCanceled()
{
  qDebug() << "App canceled";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onExited()
{
  qDebug() << "App exited";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onReleaseAvailableResources()
{
  qDebug() << "Should release resources put at the disposition of the app";
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkExampleDicomHost::getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData)
{
  Q_UNUSED(includeBulkData)
  Q_UNUSED(acceptableTransferSyntaxUIDs)

  //stupid test: only works with one uuid
  QList<ctkDicomAppHosting::ObjectLocator> locators;
  QUuid uuid;
  QUuid testUuid("{11111111-1111-1111-1111-111111111111}");
  foreach(uuid, objectUUIDs)
    {
    //stupid test: only works with one uuid
    if (uuid == testUuid)
      {
      ctkDicomAppHosting::ObjectLocator objectLocator;
      objectLocator.locator = QUuid();
      objectLocator.source = QUuid();
      //need to filter transfert syntax with acceptable ones
      objectLocator.transferSyntax = "transSyntaxUId";
      objectLocator.length = 0;
      objectLocator.offset = 0;
      objectLocator.URI = "testFile.txt";
      locators.append (objectLocator);
      }
    return locators;
    }

  return QList<ctkDicomAppHosting::ObjectLocator>();
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::releaseData(const QList<QUuid>& objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}

void ctkExampleDicomHost::exitApplication()
{
  this->exitingApplication=true;
  getDicomAppService ()->setState (ctkDicomAppHosting::CANCELED);
}
