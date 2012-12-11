// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QModelIndex>
#include <QTreeView>
#include <QItemSelectionModel>

// ctk includes
#include "ctkDICOMHostMainLogic.h"
#include "ctkHostedAppPlaceholderWidget.h"
#include "ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMDatabase.h"
#include "ctkDicomAvailableDataHelper.h"

ctkDICOMHostMainLogic::ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget* placeHolder, ctkDICOMAppWidget* dicomAppWidget, 
                                             QWidget* placeHolderForControls) : 
  QObject(placeHolder), 
  PlaceHolderForHostedApp(placeHolder),
  DicomAppWidget(dicomAppWidget),
  PlaceHolderForControls(placeHolderForControls),
  ValidSelection(false),
  SendData(false)
{
  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp);
  this->HostControls = new ctkExampleHostControlWidget(Host, PlaceHolderForControls);

  Data = new ctkDicomAppHosting::AvailableData;

  disconnect(this->Host,SIGNAL(startProgress()),this->Host,SLOT(onStartProgress()));
  connect(this->Host,SIGNAL(appReady()),this,SLOT(onAppReady()), Qt::QueuedConnection);
  connect(this->Host,SIGNAL(startProgress()),this,SLOT(publishSelectedData()), Qt::QueuedConnection);
  connect(this->PlaceHolderForHostedApp,SIGNAL(resized()),this,SLOT(placeHolderResized()));

  QTreeView * treeview = dicomAppWidget->findChild<QTreeView*>("TreeView");
  QItemSelectionModel* selectionmodel = treeview->selectionModel();
  connect(selectionmodel, SIGNAL(selectionChanged ( const QItemSelection &, const QItemSelection & )),
    this, SLOT(onTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));

  connect(this->Host, SIGNAL(dataAvailable()), this, SLOT(onDataAvailable()));

  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()) );
}

ctkDICOMHostMainLogic::~ctkDICOMHostMainLogic()
{
  delete Data;
}

void ctkDICOMHostMainLogic::aboutToQuit()
{
  this->Host->exitApplicationBlocking();

  delete this->Host;
  this->Host = 0;
}

void ctkDICOMHostMainLogic::configureHostedApp()
{
  //qDebug() << "load button clicked";
  AppFileName = QFileDialog::getOpenFileName(PlaceHolderForHostedApp,"Choose hosted application",QApplication::applicationDirPath());
  HostControls->setAppFileName(AppFileName);
  emit SelectionValid(((this->Host) && (this->HostControls->validAppFileName()) && (ValidSelection)));
}

void ctkDICOMHostMainLogic::sendDataToHostedApp()
{
 if ((this->Host) && (this->HostControls->validAppFileName()) && (ValidSelection))
  {
    *Data = ctkDicomAppHosting::AvailableData(); // empty AvailableData structure (at least not with the same id...)
    foreach (const QString &str, SelectedFiles) {
      if (str.isEmpty())
        continue;
      qDebug() << str;

      ctkDicomAvailableDataHelper::addToAvailableData(*Data, 
        Host->objectLocatorCache(), 
        str);
    }
 
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

void ctkDICOMHostMainLogic::onAppReady()
{
  emit SelectionValid(ValidSelection);
  if(SendData)
  {
    bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    qDebug() << "  setState(INPROGRESS) returned: " << reply;

    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

void ctkDICOMHostMainLogic::onTreeSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    ValidSelection=false;

    if(selected.indexes().count() > 0)
    {
      foreach (const QModelIndex &index, selected.indexes()) {

        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));
        QModelIndex index0 = index.sibling(index.row(), 0);

        if(model && (model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType)))
        {
          QString s = "Series selected: ";
          QString seriesUID = model->data(index0,ctkDICOMModel::UIDRole).toString();
          s.append(seriesUID);
          SelectedFiles = DicomAppWidget->database()->filesForSeries(seriesUID);
          emit TreeSelectionChanged(s);
          ValidSelection=true;
        }
      }
    }
    if (ValidSelection==false)
      emit TreeSelectionChanged("no series selected");
    emit SelectionValid(((this->Host) && (this->HostControls->validAppFileName()) && (ValidSelection)));
}

//----------------------------------------------------------------------------
void ctkDICOMHostMainLogic::publishSelectedData()
{
  if(SendData)
  {
    qDebug()<<"send dataDescriptors";
    bool success = Host->publishData(*Data, true);
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
void ctkDICOMHostMainLogic::placeHolderResized()
{
  ///the following does not work (yet)
  //if((this->Host) && (this->Host->getApplicationState() != ctkDicomAppHosting::EXIT))
  //{
  //  QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
  //  this->Host->getDicomAppService()->bringToFront(rect);
  //}
}

//----------------------------------------------------------------------------
void ctkDICOMHostMainLogic::onDataAvailable()
{
  const ctkDicomAppHosting::AvailableData& data = this->Host->getIncomingAvailableData();
  QList<QUuid> uuidlist = ctkDicomAvailableDataHelper::getAllUuids(data);

  if(uuidlist.count()==0)
    return;

  QString transfersyntax("1.2.840.10008.1.2.1");
  QList<QString> transfersyntaxlist;
  transfersyntaxlist.append(transfersyntax);
  QList<ctkDicomAppHosting::ObjectLocator> locators;
  locators = this->Host->getOtherSideExchangeService()->getData(uuidlist, transfersyntaxlist, false);
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
  }
}