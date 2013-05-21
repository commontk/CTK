// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QModelIndex>
#include <QTreeView>
#include <QItemSelectionModel>

// ctk includes
#include "ctkExampleHostLogic.h"
#include "ctkHostedAppPlaceholderWidget.h"
#include "ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAvailableDataHelper.h"

ctkExampleHostLogic::ctkExampleHostLogic(ctkHostedAppPlaceholderWidget* placeHolder, 
                                             QWidget* placeHolderForControls, int hostPort, int appPort) : 
  QObject(placeHolder), 
  PlaceHolderForHostedApp(placeHolder),
  PlaceHolderForControls(placeHolderForControls),
//  ValidSelection(false),
  LastData(false),
  SendData(false)
{
  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp, hostPort, appPort);
  this->HostControls = new ctkExampleHostControlWidget(Host, PlaceHolderForControls);

  Data = new ctkDicomAppHosting::AvailableData;

  disconnect(this->Host,SIGNAL(startProgress()),this->Host,SLOT(onStartProgress()));
  connect(this->Host,SIGNAL(appReady()),this,SLOT(onAppReady()), Qt::QueuedConnection);
  connect(this->Host,SIGNAL(startProgress()),this,SLOT(publishSelectedData()), Qt::QueuedConnection);
  connect(this->PlaceHolderForHostedApp,SIGNAL(resized()),this,SLOT(placeHolderResized()));

  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()) );
}

ctkExampleHostLogic::~ctkExampleHostLogic()
{
  delete Data;
}

void ctkExampleHostLogic::aboutToQuit()
{
  this->Host->exitApplicationBlocking();

  delete this->Host;
  this->Host = 0;
}

void ctkExampleHostLogic::configureHostedApp()
{
  //qDebug() << "load button clicked";
  AppFileName = QFileDialog::getOpenFileName(PlaceHolderForHostedApp,"Choose hosted application",QApplication::applicationDirPath());
  HostControls->setAppFileName(AppFileName);
//  emit SelectionValid(((this->Host) && (this->HostControls->validAppFileName()) && (ValidSelection)));
}

void ctkExampleHostLogic::sendData(ctkDicomAppHosting::AvailableData& data, bool lastData)
{
 if ((this->Host) && (this->HostControls->validAppFileName()) /*&& (ValidSelection)*/)
  {
    *Data = data;
    LastData = lastData;
 
    SendData = true;
    if(this->Host->getApplicationState() == ctkDicomAppHosting::EXIT)
    {
      this->HostControls->StartApplication(this->AppFileName);
    }
    if(this->Host->getApplicationState() == ctkDicomAppHosting::IDLE)
    {
      bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    }
    if(this->Host->getApplicationState() == ctkDicomAppHosting::INPROGRESS)
    {
      publishSelectedData();
    }
  }
}

void ctkExampleHostLogic::onAppReady()
{
//  emit SelectionValid(ValidSelection);
  if(SendData)
  {
    bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    qDebug() << "  setState(INPROGRESS) returned: " << reply;

    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

//----------------------------------------------------------------------------
void ctkExampleHostLogic::publishSelectedData()
{
  if(SendData)
  {
    qDebug()<<"send dataDescriptors";
    bool success = Host->publishData(*Data, LastData);
    if(!success)
    {
      qCritical() << "Failed to publish data";
    }
    qDebug() << "  notifyDataAvailable returned: " << success;
    SendData=false;

    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

//----------------------------------------------------------------------------
void ctkExampleHostLogic::placeHolderResized()
{
  ///the following does not work (yet)
  if((this->Host) && (this->Host->getApplicationState() != ctkDicomAppHosting::EXIT))
  {
    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

//----------------------------------------------------------------------------
ctkExampleDicomHost* ctkExampleHostLogic::getHost()
{
  return this->Host;
}

//----------------------------------------------------------------------------
ctkExampleHostControlWidget* ctkExampleHostLogic::getHostControls()
{
  return this->HostControls;
}
