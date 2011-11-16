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

ctkDICOMHostMainLogic::ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget* placeHolder, ctkDICOMAppWidget* dicomAppWidget) : 
  QObject(placeHolder), 
  PlaceHolderForHostedApp(placeHolder),
  DicomAppWidget(dicomAppWidget),
  ValidSelection(false)
{
  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp);
  this->HostControls = new ctkExampleHostControlWidget(Host, placeHolder);
  this->HostControls->show();

  disconnect(this->Host,SIGNAL(startProgress()),this->Host,SLOT(onStartProgress()));
  connect(this->Host,SIGNAL(startProgress()),this,SLOT(onStartProgress()));

  QTreeView * treeview = dicomAppWidget->findChild<QTreeView*>("TreeView");
  QItemSelectionModel* selectionmodel = treeview->selectionModel();
  connect(selectionmodel, SIGNAL(selectionChanged ( const QItemSelection &, const QItemSelection & )),
    this, SLOT(onTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

ctkDICOMHostMainLogic::~ctkDICOMHostMainLogic()
{
}

void ctkDICOMHostMainLogic::configureHostedApp()
{
  //qDebug() << "load button clicked";
  AppFileName = QFileDialog::getOpenFileName(PlaceHolderForHostedApp,"Choose hosted application",QApplication::applicationDirPath());
  //HostControls->setAppFileName(name);
  //if (this->Host)
  //  {
  //  this->Host->StartApplication(this->AppFileName);
  //  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
  //  qDebug() << "  setState(INPROGRESS) returned: " << reply;
  //  }
  HostControls->setAppFileName(AppFileName);
}

void ctkDICOMHostMainLogic::sendDataToHostedApp()
{
  //if(Host->
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
}

//----------------------------------------------------------------------------
void ctkDICOMHostMainLogic::onStartProgress()
{
  if(ValidSelection)
  {
    ctkDicomAppHosting::AvailableData data;
    foreach (const QString &str, SelectedFiles) {
      if (str.isEmpty())
        continue;
      qDebug() << str;

      ctkDicomAvailableDataHelper::addToAvailableData(data, 
        Host->objectLocatorCache(), 
        str);
    }

    qDebug()<<"send dataDescriptors";
    bool success = Host->publishData(data, true);
    if(!success)
    {
      qCritical() << "Failed to publish data";
    }
    qDebug() << "  notifyDataAvailable returned: " << success;
  }
}
