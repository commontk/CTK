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


#include "ctkExampleDicomAppLogic_p.h"
#include "ctkExampleDicomAppPlugin_p.h"

// Qt includes
#include <QtPlugin>
#include <QRect>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QLabel>

// ctkDICOMCore includes
#include "ctkDICOMImage.h"

// DCMTK includes
#include <dcmimage.h>

ctkExampleDicomAppLogic::ctkExampleDicomAppLogic()
  : hostTracker(ctkExampleDicomAppPlugin::getPluginContext()), button(NULL)
{
  hostTracker.open();

  connect(this, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)), Qt::QueuedConnection);
  emit stateChanged(ctkDicomAppHosting::IDLE);
}

ctkExampleDicomAppLogic::~ctkExampleDicomAppLogic()
{
  ctkPluginContext* context = ctkExampleDicomAppPlugin::getPluginContext();
  QList <QSharedPointer<ctkPlugin> > plugins = context->getPlugins();
  for (int i = 0; i < plugins.size(); ++i)
  {
    qDebug() << plugins.at(i)->getSymbolicName ();
  }
}

ctkDicomAppHosting::State ctkExampleDicomAppLogic::getState()
{
  return ctkDicomAppHosting::IDLE;
}

bool ctkExampleDicomAppLogic::setState(ctkDicomAppHosting::State newState)
{
  qDebug() << "setState called";
  emit stateChanged(newState);
  return true;
}

bool ctkExampleDicomAppLogic::bringToFront(const QRect& /*requestedScreenArea*/)
{
  return false;
}

void ctkExampleDicomAppLogic::do_something()
{
  button = new QPushButton("Button from App");
  connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  try
  {

    QRect preferred(50,50,100,100);
    qDebug() << "  Asking:getAvailableScreen";
    QRect rect = getHostInterface()->getAvailableScreen(preferred);
    qDebug() << "  got sth:" << rect.top();
    button->move(rect.topLeft());
    button->resize(rect.size());
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
    return;
  }
  button->show();
}

void ctkExampleDicomAppLogic::changeState(int anewstate)
{
  ctkDicomAppHosting::State newstate = static_cast<ctkDicomAppHosting::State>(anewstate);

  if (newstate == ctkDicomAppHosting::INPROGRESS)
  {
    do_something();
  }

  try
  {
    getHostInterface()->notifyStateChanged(newstate);
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
    return;
  }

  if (newstate == ctkDicomAppHosting::CANCELED)
  {
    qDebug() << "  Received changeState(CANCELED) ... now releasing all resources and afterwards changing to state IDLE.";
    qDebug() << "  Changing to state IDLE.";
    try
    {
      getHostInterface()->notifyStateChanged(ctkDicomAppHosting::IDLE);
    }
    catch (const std::runtime_error& e)
    {
      qCritical() << e.what();
      return;
    }
  }

  if (newstate == ctkDicomAppHosting::EXIT)
  {
    qDebug() << "  Received changeState(EXIT) ... exiting.";
    getHostInterface()->notifyStateChanged(ctkDicomAppHosting::EXIT);
    qApp->exit(0);
  }
}

bool ctkExampleDicomAppLogic::notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData)
{
  Q_UNUSED(lastData)
  QString s;
  if(button==NULL)
  {
    qCritical() << "Button is null!";
    return false;
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
        uuid = data.patients.begin()->studies.begin()->series.begin()->objectDescriptors.begin()->descriptorUUID;
        s=s+" uuid:"+uuid.toString();
      }
    }
  }
  button->setText(s);
  return false;
}

QList<ctkDicomAppHosting::ObjectLocator> ctkExampleDicomAppLogic::getData(
  QList<QUuid> objectUUIDs,
  QList<QString> acceptableTransferSyntaxUIDs,
  bool includeBulkData)
{
  Q_UNUSED(objectUUIDs)
  Q_UNUSED(acceptableTransferSyntaxUIDs)
  Q_UNUSED(includeBulkData)
  return QList<ctkDicomAppHosting::ObjectLocator>();
}

void ctkExampleDicomAppLogic::releaseData(QList<QUuid> objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}

ctkDicomHostInterface* ctkExampleDicomAppLogic::getHostInterface() const
{
  ctkDicomHostInterface* host = hostTracker.getService();
  if (!host) throw std::runtime_error("DICOM Host Interface not available");
  return host;
}

void ctkExampleDicomAppLogic::buttonClicked()
{
  QList<QUuid> uuidlist;
  uuidlist.append(uuid);
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
    s=s+" URI: "+locators.begin()->URI;
    qDebug() << "URI: " << locators.begin()->URI;
    QString filename = locators.begin()->URI;
    if(filename.startsWith("file:/",Qt::CaseInsensitive))
      filename=filename.remove(0,6);
    qDebug()<<filename;
    DicomImage dcmtkImage(filename.toLatin1().data());
    ctkDICOMImage ctkImage(&dcmtkImage);

    QLabel* qtImage = new QLabel;
    qtImage->setPixmap(ctkImage.getPixmap(0));
    qtImage->show();
  }
  button->setText(s);

}
