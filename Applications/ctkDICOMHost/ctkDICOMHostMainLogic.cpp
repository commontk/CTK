// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QApplication>

// ctk includes
#include "ctkDICOMHostMainLogic.h"
#include "ctkHostedAppPlaceholderWidget.h"
#include "ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"

ctkDICOMHostMainLogic::ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget* placeHolder) : 
  QObject(placeHolder), 
  placeHolderForHostedApp(placeHolder)
{
  this->Host = new ctkExampleDicomHost(placeHolderForHostedApp);
  this->HostControls = new ctkExampleHostControlWidget(Host, placeHolder);
  this->HostControls->show();
  //connect(placeHolder,SIGNAL(resized()),SLOT(placeholderResized()));
}

ctkDICOMHostMainLogic::~ctkDICOMHostMainLogic()
{
}

void ctkDICOMHostMainLogic::configureHostedApp()
{
  //qDebug() << "load button clicked";
  AppFileName = QFileDialog::getOpenFileName(placeHolderForHostedApp,"Choose hosted application",QApplication::applicationDirPath());
  //HostControls->setAppFileName(name);
  //if (this->Host)
  //  {
  //  this->Host->StartApplication(this->AppFileName);
  //  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
  //  qDebug() << "  setState(INPROGRESS) returned: " << reply;
  //  }
  HostControls->setAppFileName(AppFileName);
}
