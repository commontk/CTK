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
#include "ctkDicomAvailableDataHelper.h"

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
  ctkDicomAppHosting::AvailableData data;
  ctkDicomAvailableDataHelper::addToAvailableData(data, 
    this->objectLocatorCache(), 
    "C:/XIP/XIPHost/dicom-dataset-demo/1.3.6.1.4.1.9328.50.1.10698.dcm");

  qDebug()<<"send dataDescriptors";
  bool success = this->publishData(data, true);
  if(!success)
  {
    qCritical() << "Failed to publish data";
  }
  qDebug() << "  notifyDataAvailable(1111) returned: " << success;
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
void ctkExampleDicomHost::releaseData(const QList<QUuid>& objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}

void ctkExampleDicomHost::exitApplication()
{
  this->exitingApplication=true;
  getDicomAppService ()->setState (ctkDicomAppHosting::CANCELED);
}
