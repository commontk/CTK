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
#include <QStringList>
#include <QDir>

// CTK includes
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppHostingTypesHelper.h"
#include "ctkDicomAvailableDataHelper.h"

// STD includes
#include <iostream>

// DCMTK includes
#include <dcmtk/dcmdata/dcuid.h>

//----------------------------------------------------------------------------
ctkExampleDicomHost::ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort, int appPort) :
    ctkDicomAbstractHost(hostPort, appPort),
    PlaceholderWidget(placeholderWidget),
    exitingApplication(false)
{
  connect(this,SIGNAL(appReady()),SLOT(onAppReady()), Qt::QueuedConnection);
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
  if(this->AppProcess.state()!=QProcess::NotRunning)
  {
    qDebug() << "Exiting host: trying to terminate app";
    this->AppProcess.terminate();
    qDebug() << "Exiting host: trying to kill app";
    this->AppProcess.kill();
  }
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
  cleanIncomingData();
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onReleaseAvailableResources()
{
  qDebug() << "Should release resources put at the disposition of the app";
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::exitApplication()
{
  this->exitingApplication=true;
  if(this->getAppProcess().state() == QProcess::Running)
  {
    if(this->getApplicationState() == ctkDicomAppHosting::EXIT)
      return;
    if(this->getApplicationState() == ctkDicomAppHosting::IDLE)
    {
      getDicomAppService ()->setState (ctkDicomAppHosting::EXIT);
      return;
    }
    getDicomAppService ()->setState (ctkDicomAppHosting::CANCELED);
  }
}

//----------------------------------------------------------------------------
QString ctkExampleDicomHost::getOutputLocation(const QStringList& preferredProtocols)
{ 
  if(preferredProtocols.indexOf("file:")>=0)
    return QDir::temp().absolutePath();

  return ""; 
}

//----------------------------------------------------------------------------
ctkDicomAppHosting::State ctkExampleDicomHost::getApplicationState()const
{
  if(this->getAppProcess().state() == QProcess::NotRunning)
    return ctkDicomAppHosting::EXIT;
  return ctkDicomAbstractHost::getApplicationState();
}

//----------------------------------------------------------------------------
QString ctkExampleDicomHost::generateUID()
{
  char uid[100];
  dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
  return uid;
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::exitApplicationBlocking(int timeout)
{
  connect(&this->AppProcess,SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(onBlockingExiting(QProcess::ProcessState)));

  if(this->getApplicationState() != ctkDicomAppHosting::EXIT)
  {
    this->exitApplication();
    QTimer::singleShot(timeout,this,SLOT(onBlockingExiting()));
    BlockingLoopForExiting.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
  }
  if(this->AppProcess.state()!=QProcess::NotRunning)
  {
    this->AppProcess.kill();
  }
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onBlockingExiting(QProcess::ProcessState)
{
  this->AppProcess.disconnect(SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onBlockingExiting(QProcess::ProcessState)));
  BlockingLoopForExiting.exit(0);
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::onBlockingExiting()
{
  BlockingLoopForExiting.exit(0);
}
