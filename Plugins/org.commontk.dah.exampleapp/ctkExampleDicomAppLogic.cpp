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
#include <QtPlugin>
#include <QRect>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QRect>
#include <QStringList>
#include <QDir>
#include <QTemporaryFile>
#include <QPainter>

// CTK includes
#include "ctkDICOMImage.h"
#include "ctkExampleDicomAppLogic_p.h"
#include "ctkExampleDicomAppPlugin_p.h"
#include "ctkDicomAvailableDataHelper.h"

// DCMTK includes
#include <dcmtk/dcmimgle/dcmimage.h>

//----------------------------------------------------------------------------
ctkExampleDicomAppLogic::ctkExampleDicomAppLogic():
ctkDicomAbstractApp(ctkExampleDicomAppPlugin::getPluginContext()), AppWidget(0)
{


  connect(this, SIGNAL(startProgress()), this, SLOT(onStartProgress()), Qt::QueuedConnection);
  connect(this, SIGNAL(resumeProgress()), this, SLOT(onResumeProgress()), Qt::QueuedConnection);
  connect(this, SIGNAL(suspendProgress()), this, SLOT(onSuspendProgress()), Qt::QueuedConnection);
  connect(this, SIGNAL(cancelProgress()), this, SLOT(onCancelProgress()), Qt::QueuedConnection);
  connect(this, SIGNAL(exitHostedApp()), this, SLOT(onExitHostedApp()), Qt::QueuedConnection);
  connect(this, SIGNAL(dataAvailable()), this, SLOT(onDataAvailable()));

  //notify Host we are ready.
  try {
    getHostInterface()->notifyStateChanged(ctkDicomAppHosting::IDLE);
  }
  catch(...)
  {
    qDebug() << "ctkDicomAbstractApp: Could not getHostInterface()";
  }
}

//----------------------------------------------------------------------------
ctkExampleDicomAppLogic::~ctkExampleDicomAppLogic()
{
  ctkPluginContext* context = ctkExampleDicomAppPlugin::getPluginContext();
  QList <QSharedPointer<ctkPlugin> > plugins = context->getPlugins();
  for (int i = 0; i < plugins.size(); ++i)
  {
    qDebug() << plugins.at(i)->getSymbolicName ();
  }
}

//----------------------------------------------------------------------------
bool ctkExampleDicomAppLogic::bringToFront(const QRect& requestedScreenArea)
{
  if(this->AppWidget!=NULL)
  {
    this->AppWidget->move(requestedScreenArea.topLeft());
    this->AppWidget->resize(requestedScreenArea.size());
    this->AppWidget->activateWindow();
    this->AppWidget->raise();
  }
  return true;
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::do_something()
{
  AppWidget = new QWidget;
  ui.setupUi(AppWidget);

  connect(ui.LoadDataButton, SIGNAL(clicked()), this, SLOT(onLoadDataClicked()));
  connect(ui.CreateSecondaryCaptureButton, SIGNAL(clicked()), this, SLOT(onCreateSecondaryCapture()));
  try
    {
    QRect preferred(50,50,100,100);
    qDebug() << "  Asking:getAvailableScreen";
    QRect rect = getHostInterface()->getAvailableScreen(preferred);
    qDebug() << "  got sth:" << rect.top();
    this->AppWidget->move(rect.topLeft());
    this->AppWidget->resize(rect.size());
    }
  catch (const ctkRuntimeException& e)
    {
    qCritical() << e.what();
    return;
    }
  this->AppWidget->show();
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onStartProgress()
{
  setInternalState(ctkDicomAppHosting::INPROGRESS);

  // we need to create the button before we receive data from
  // the host, which happens immediately after calling
  // getHostInterface()->notifyStateChanged
  do_something(); 

  getHostInterface()->notifyStateChanged(ctkDicomAppHosting::INPROGRESS);
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onResumeProgress()
{
  //reclame all resources.

  //notify state changed
  setInternalState(ctkDicomAppHosting::INPROGRESS);
  getHostInterface()->notifyStateChanged(ctkDicomAppHosting::INPROGRESS);
  //we're rolling
  //do something else normally, but this is an example
  ui.LoadDataButton->setEnabled(true);
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onSuspendProgress()
{
  //release resources it can reclame later to resume work
  ui.LoadDataButton->setEnabled(false);
  //notify state changed
  setInternalState(ctkDicomAppHosting::SUSPENDED);
  getHostInterface()->notifyStateChanged(ctkDicomAppHosting::SUSPENDED);
  //we're rolling
  //do something else normally, but this is an example
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onCancelProgress()
{
  //release all resources
  onReleaseResources();
  //update state
  setInternalState(ctkDicomAppHosting::IDLE);
  getHostInterface()->notifyStateChanged(ctkDicomAppHosting::IDLE);
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onExitHostedApp()
{
  //useless move, but correct:
  setInternalState(ctkDicomAppHosting::EXIT);
  getHostInterface()->notifyStateChanged(ctkDicomAppHosting::EXIT);
  qDebug() << "Exiting";
  //die
  qApp->exit(0);
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onReleaseResources()
{
  this->AppWidget->hide();
  delete (this->AppWidget);
  this->AppWidget = 0 ;
}


//----------------------------------------------------------------------------
void ctkExampleDicomAppLogic::onDataAvailable()
{
  QString s;
  const ctkDicomAppHosting::AvailableData& data = getIncomingAvailableData();
  if(this->AppWidget == 0)
    {
    qCritical() << "Button is null!";
    return;
    }
  s = "Received notifyDataAvailable with patients.count()= " + QString().setNum(data.patients.count());
  if(data.patients.count()>0)
    {
    s=s+" name:"+data.patients.begin()->name+" studies.count(): "+QString().setNum(data.patients.begin()->studies.count());
    if(data.patients.begin()->studies.count()>0)
    {
      s=s+" series.count():" + QString().setNum(data.patients.begin()->studies.begin()->series.count());
      if(data.patients.begin()->studies.begin()->series.count()>0)
      {
        s=s+" uid:" + data.patients.begin()->studies.begin()->series.begin()->seriesUID;
//        QUuid uuid("93097dc1-caf9-43a3-a814-51a57f8d861d");//data.patients.begin()->studies.begin()->series.begin()->seriesUID);
        QUuid uuid = data.patients.begin()->studies.begin()->series.begin()->objectDescriptors.begin()->descriptorUUID;
        s=s+" uuid:"+uuid.toString();
      }
    }
  }
  else
  {
    s = s+", objectDescriptors.count()= " + QString().setNum(data.objectDescriptors.count());
  }
  ui.ReceivedDataInformation->setText(s);
  ui.LoadDataButton->setEnabled(true);
}


void ctkExampleDicomAppLogic::onLoadDataClicked()
{
  const ctkDicomAppHosting::AvailableData& data = getIncomingAvailableData();
  QList<QUuid> uuidlist;
  if(data.patients.count()!=0)
  {
    const ctkDicomAppHosting::Patient& firstpatient = *data.patients.begin();
    uuidlist = ctkDicomAvailableDataHelper::getAllUuids(firstpatient);
  }
  else if(data.objectDescriptors.count()!=0)
  {
    uuidlist = ctkDicomAvailableDataHelper::getAllUuids(data);
  }
  else
    return;
  
  QString transfersyntax("1.2.840.10008.1.2.1");
  QList<QString> transfersyntaxlist;
  transfersyntaxlist.append(transfersyntax);
  QList<ctkDicomAppHosting::ObjectLocator> locators;
  locators = getHostInterface()->getData(uuidlist, transfersyntaxlist, false);
  qDebug() << "got locators! " << QString().setNum(locators.count());

  QString s;
  s=s+" loc.count:"+QString().setNum(locators.count());
  if(locators.count()>0)
  {
    s=s+" URI: "+locators.begin()->URI +" locatorUUID: "+locators.begin()->locator+" sourceUUID: "+locators.begin()->source;
    qDebug() << "URI: " << locators.begin()->URI;
    QString filename = locators.begin()->URI;
    if(filename.startsWith("file:/",Qt::CaseInsensitive))
      filename=filename.remove(0,8);
    qDebug()<<filename;
    if(QFileInfo(filename).exists())
    {
      try {
        DicomImage dcmtkImage(filename.toUtf8().data());
        ctkDICOMImage ctkImage(&dcmtkImage);

        QPixmap pixmap = QPixmap::fromImage(ctkImage.frame(0),Qt::AvoidDither);
        if (pixmap.isNull())
        {
          qCritical() << "Failed to convert QImage to QPixmap" ;
        }
        else
        {
          ui.PlaceHolderForImage->setPixmap(pixmap);
        }
      }
      catch(...)
      {
        qCritical() << "Caught exception while trying to load file" << filename;
      }
    }
    else
    {
      qCritical() << "File does not exist: " << filename;
    }
  }
  ui.ReceivedDataInformation->setText(s);
}

void ctkExampleDicomAppLogic::onCreateSecondaryCapture()
{
  const QPixmap* pixmap = ui.PlaceHolderForImage->pixmap();
  if(pixmap!=NULL)
  {
    QStringList preferredProtocols;
    preferredProtocols.append("file:");
    QString outputlocation = getHostInterface()->getOutputLocation(preferredProtocols);
    QString templatefilename = QDir(outputlocation).absolutePath();
    if(templatefilename.isEmpty()==false) templatefilename.append('/'); 
    templatefilename.append("ctkdahscXXXXXX.jpg");
    QTemporaryFile *tempfile = new QTemporaryFile(templatefilename,this->AppWidget);

    if(tempfile->open())
    {
      QString filename = QFileInfo(tempfile->fileName()).absoluteFilePath();
      qDebug() << "Created file: " << filename;
      tempfile->close();
      QPixmap tmppixmap(*pixmap);
      QPainter painter(&tmppixmap);
      painter.setPen(Qt::white);
      painter.setFont(QFont("Arial", 15));
      painter.drawText(tmppixmap.rect(),Qt::AlignBottom|Qt::AlignLeft,"Secondary capture by ctkExampleDicomApp");
     //painter.drawText(rect(), Qt::AlignCenter, "Qt");
      tmppixmap.save(tempfile->fileName(), "JPEG");
      qDebug() << "Created Uuid: " << getHostInterface()->generateUID();

      ctkDicomAppHosting::AvailableData resultData;
      ctkDicomAvailableDataHelper::addToAvailableData(resultData, 
        objectLocatorCache(), 
        tempfile->fileName());

      bool success = publishData(resultData, true);
      if(!success)
      {
        qCritical() << "Failed to publish data";
      }
      qDebug() << "  publishData returned: " << success;

    }
    else
      qDebug() << "Creating temporary file failed.";
  }

}
