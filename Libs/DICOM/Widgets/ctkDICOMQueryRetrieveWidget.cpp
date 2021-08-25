/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QTreeView>
#include <QTabBar>

/// CTK includes
#include <ctkCheckableHeaderView.h>
#include <ctkDICOMTableManager.h>
#include <ctkCheckableModelHelper.h>
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMQuery.h"
#include "ctkDICOMRetrieve.h"

// ctkDICOMWidgets includes
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMQueryRetrieveWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMQueryRetrieveWidget");

//----------------------------------------------------------------------------
class ctkDICOMQueryRetrieveWidgetPrivate: public Ui_ctkDICOMQueryRetrieveWidget
{
  Q_DECLARE_PUBLIC( ctkDICOMQueryRetrieveWidget );

protected:
  ctkDICOMQueryRetrieveWidget* const q_ptr;
  
public:
  ctkDICOMQueryRetrieveWidgetPrivate(ctkDICOMQueryRetrieveWidget& obj);
  ~ctkDICOMQueryRetrieveWidgetPrivate();
  void init();

  QMap<QString, ctkDICOMQuery*>     QueriesByServer;
  QMap<QString, ctkDICOMQuery*>     QueriesByStudyUID;
  QMap<QString, ctkDICOMRetrieve*>  RetrievalsByStudyUID;
  ctkDICOMDatabase                  QueryResultDatabase;
  QSharedPointer<ctkDICOMDatabase>  RetrieveDatabase;
  ctkDICOMModel                     Model;
  ctkDICOMQuery                     *CurrentQuery;
  
  QProgressDialog*                  ProgressDialog;
  QString                           CurrentServer;
    bool                              UseProgressDialog;
};

//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidgetPrivate::ctkDICOMQueryRetrieveWidgetPrivate(
  ctkDICOMQueryRetrieveWidget& obj)
  : q_ptr(&obj)
{
  this->ProgressDialog = 0;
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidgetPrivate::~ctkDICOMQueryRetrieveWidgetPrivate()
{
  foreach(ctkDICOMQuery* query, this->QueriesByServer.values())
    {
    delete query;
    }
  foreach(ctkDICOMRetrieve* retrieval, this->RetrievalsByStudyUID.values())
    {
    delete retrieval;
    }
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidgetPrivate::init()
{
  Q_Q(ctkDICOMQueryRetrieveWidget);
  this->setupUi(q);

  QObject::connect(this->QueryWidget, SIGNAL(returnPressed()), q, SLOT(query()));
  QObject::connect(this->QueryButton, SIGNAL(clicked()), q, SLOT(query()));
  QObject::connect(this->RetrieveButton, SIGNAL(clicked()), q, SLOT(retrieve()));
  QObject::connect(this->CancelButton, SIGNAL(clicked()), q, SLOT(cancel()));

}

//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::ctkDICOMQueryRetrieveWidget(QWidget* parentWidget)
  : Superclass(parentWidget) 
  , d_ptr(new ctkDICOMQueryRetrieveWidgetPrivate(*this))
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::~ctkDICOMQueryRetrieveWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::setRetrieveDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  d->RetrieveDatabase = dicomDatabase;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMQueryRetrieveWidget::retrieveDatabase()const
{
  Q_D(const ctkDICOMQueryRetrieveWidget);
  return d->RetrieveDatabase;
}

void ctkDICOMQueryRetrieveWidget::useProgressDialog(bool enable)
{
    Q_D(ctkDICOMQueryRetrieveWidget);
    d->UseProgressDialog=enable;
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::query()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  d->RetrieveButton->setEnabled(false);

  if (!d->QueryResultDatabase.isOpen())
  {
    // create a database in memory to hold query results
    try
    {
      d->QueryResultDatabase.openDatabase(":memory:");
    }
    catch (std::exception e)
    {
      logger.error("Database error: " + d->QueryResultDatabase.lastError());
      d->QueryResultDatabase.closeDatabase();
      return;
    }
  }

  // Clear the database and set schema.
  // Use a special schema that works well with fields received in query results
  // and does not rely on displayed field setting.
  // (Current limitation is that displayed fields cannot be computed if files
  // are not inserted into the database).
  d->QueryResultDatabase.initializeDatabase(":/dicom/dicom-qr-schema.sql");

  d->QueriesByStudyUID.clear();
  // for each of the selected server nodes, send the query
  QProgressDialog progress("Query DICOM servers", "Cancel", 0, 100, this,
                           Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
  // We don't want the progress dialog to resize itself, so we bypass the label
  // by creating our own
  QLabel* progressLabel = new QLabel(tr("Initialization..."));
  progress.setLabel(progressLabel);
  d->ProgressDialog = &progress;
  progress.setWindowModality(Qt::ApplicationModal);
  progress.setMinimumDuration(0);
  progress.setValue(0);
  progress.show();
  foreach (d->CurrentServer, d->ServerNodeWidget->selectedServerNodes())
    {
    if (progress.wasCanceled())
      {
      break;
      }
    QMap<QString, QVariant> parameters =
      d->ServerNodeWidget->serverNodeParameters(d->CurrentServer);
    // if we are here it's because the server node was checked
    Q_ASSERT(parameters["CheckState"] == static_cast<int>(Qt::Checked) );
    // create a query for the current server
    ctkDICOMQuery* query = new ctkDICOMQuery;
    d->CurrentQuery = query;
    query->setCallingAETitle(d->ServerNodeWidget->callingAETitle());
    query->setCalledAETitle(parameters["AETitle"].toString());
    query->setHost(parameters["Address"].toString());
    query->setPort(parameters["Port"].toInt());
    query->setPreferCGET(parameters["CGET"].toBool());

    // populate the query with the current search options
    query->setFilters( d->QueryWidget->parameters() );

    try
      {
      connect(&progress, SIGNAL(canceled()), query, SLOT(cancel()));
      connect(query, SIGNAL(progress(QString)),
              progressLabel, SLOT(setText(QString)));
      connect(query, SIGNAL(progress(int)),
              this, SLOT(onQueryProgressChanged(int)));

      // run the query against the selected server and put results in database
      bool wasBatchUpdate = d->dicomTableManager->setBatchUpdate(true);
      query->query ( d->QueryResultDatabase );
      d->dicomTableManager->setBatchUpdate(wasBatchUpdate);

      disconnect(query, SIGNAL(progress(QString)),
                 progressLabel, SLOT(setText(QString)));
      disconnect(query, SIGNAL(progress(int)),
                 this, SLOT(onQueryProgressChanged(int)));
      disconnect(&progress, SIGNAL(canceled()), query, SLOT(cancel()));
      }
    catch (std::exception e)
      {
      logger.error ( "Query error: " + parameters["Name"].toString() );
      progress.setLabelText("Query error: " + parameters["Name"].toString());
      delete query;
      }

    d->QueriesByServer[d->CurrentServer] = query;
    
    foreach( QString studyUID, query->studyInstanceUIDQueried() )
      {
      d->QueriesByStudyUID[studyUID] = query;
      }
    }
  
  if (!progress.wasCanceled())
    {
    d->Model.setDatabase(d->QueryResultDatabase.database());
    
    d->dicomTableManager->setDICOMDatabase(&(d->QueryResultDatabase));
    }
  d->RetrieveButton->setEnabled(d->QueriesByStudyUID.keys().size() != 0);

  // We would need to call database.updateDisplayedFields() now, but currently
  // updateDisplayedFields requires entries in the Image table and tag cache
  // and they are not set when inserting query results.
  // Therefore, for now we do not compute displayed fields and use a schema
  // that only shows raw DICOM values.

  progress.setValue(progress.maximum());
  d->ProgressDialog = 0;
  d->CurrentQuery = 0;
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::retrieve()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  if (!d->RetrieveButton->isEnabledTo(this))
    {
    return;
    }

  QProgressDialog progress("Retrieve from DICOM servers", "Cancel", 0, 0, this,
                           Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
  // We don't want the progress dialog to resize itself, so we bypass the label
  // by creating our own
  QLabel* progressLabel = new QLabel(tr("Initialization..."));

  // for each of the selected server nodes, send the query
  if(d->UseProgressDialog)
    {
    progress.setLabel(progressLabel);
    d->ProgressDialog = &progress;
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.setMaximum(0);
    progress.setAutoClose(false);
    progress.show();
    }

  QMap<QString,QVariant> serverParameters = d->ServerNodeWidget->parameters();
  ctkDICOMRetrieve *retrieve = new ctkDICOMRetrieve;
  // only start new association if connection parameters change
  retrieve->setKeepAssociationOpen(true);
  // pull from GUI
  retrieve->setMoveDestinationAETitle( serverParameters["StorageAETitle"].toString() );

  // do the rerieval for each selected series
  // that is selected in the tree view
  QStringList selectedSeriesUIDs = d->dicomTableManager->currentStudiesSelection();
  foreach( QString studyUID, selectedSeriesUIDs )
    {
    std::cout<<studyUID.toUtf8().constData()<<std::endl;
    if(d->UseProgressDialog)
      {
      if (progress.wasCanceled())
        {
        break;
        }
      progressLabel->setText(QString(tr("Retrieving:\n%1")).arg(studyUID));
      this->updateRetrieveProgress(0);
      }

    // Get information which server we want to get the study from and prepare request accordingly
    ctkDICOMQuery *query = d->QueriesByStudyUID[studyUID];
    retrieve->setDatabase( d->RetrieveDatabase );
    retrieve->setCallingAETitle( query->callingAETitle() );
    retrieve->setCalledAETitle( query->calledAETitle() );
    retrieve->setPort( query->port() );
    retrieve->setHost( query->host() );
    // TODO: check the model item to see if it is checked
    // for now, assume all studies queried and shown to the user will be retrieved
    logger.debug("About to retrieve " + studyUID + " from " + d->QueriesByStudyUID[studyUID]->host());
    logger.info ( "Starting to retrieve" );

    if(d->UseProgressDialog)
      {
      connect(&progress, SIGNAL(canceled()), retrieve, SLOT(cancel()));
      connect(retrieve, SIGNAL(progress(QString)),
              progressLabel, SLOT(setText(QString)));
      connect(retrieve, SIGNAL(progress(int)),
              this, SLOT(updateRetrieveProgress(int)));
      }
    try
      {
      // perform the retrieve
      if ( query->preferCGET() )
        {
        retrieve->getStudy ( studyUID );
        }
      else
        {
        retrieve->moveStudy ( studyUID );
        }
      }
    catch (std::exception e)
      {
      logger.error ( "Retrieve failed" );
      if(d->UseProgressDialog)
        {
        if ( QMessageBox::question ( this, 
              tr("Query Retrieve"), tr("Retrieve failed.  Keep trying?"),
              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
          {
          continue;
          }
        else
          {
          break;
          }
        }
      }

    if(d->UseProgressDialog)
      {
      disconnect(retrieve, SIGNAL(progress(QString)),
              progressLabel, SLOT(setText(QString)));
      disconnect(retrieve, SIGNAL(progress(int)),
              this, SLOT(updateRetrieveProgress(int)));
      disconnect(&progress, SIGNAL(canceled()), retrieve, SLOT(cancel()));
      }
    logger.info ( "Retrieve success" );
    }

  if (retrieve->database())
    {
    retrieve->database()->updateDisplayedFields();
    }

  if(d->UseProgressDialog)
    {
    QString message(tr("Retrieve Process Finished"));
    if (retrieve->wasCanceled())
      {
      message = tr("Retrieve Process Canceled");
      }
    QMessageBox::information ( this, tr("Query Retrieve"), message );
    }
  emit studiesRetrieved(d->RetrievalsByStudyUID.keys());

  delete retrieve;
  d->ProgressDialog = 0;
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::cancel()
{
  emit studiesRetrieved(QStringList());
  emit canceled();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::onQueryProgressChanged(int value)
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  if (d->ProgressDialog == 0)
    {
    return;
    }
  if (d->CurrentQuery && d->ProgressDialog->wasCanceled())
    {
    d->CurrentQuery->cancel();
    }
  QStringList servers = d->ServerNodeWidget->selectedServerNodes();
  int serverIndex = servers.indexOf(d->CurrentServer);
  if (serverIndex < 0)
    {
    return;
    }
  if (d->ProgressDialog->width() != 500)
    {
    QPoint pp = this->mapToGlobal(QPoint(0,0));
    pp = QPoint(pp.x() + (this->width() - d->ProgressDialog->width()) / 2,
                pp.y() + (this->height() - d->ProgressDialog->height())/ 2);
    d->ProgressDialog->move(pp - QPoint((500 - d->ProgressDialog->width())/2, 0));
    d->ProgressDialog->resize(500, d->ProgressDialog->height());
    }
  float serverProgress = 100. / servers.size();
  d->ProgressDialog->setValue( (serverIndex + (value / 101.)) * serverProgress);
  QApplication::processEvents();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::updateRetrieveProgress(int value)
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  if (d->ProgressDialog == 0)
    {
    return;
    }
  static int targetWidth = 700;
  if (d->ProgressDialog->width() != targetWidth)
    {
    QPoint pp = this->mapToGlobal(QPoint(0,0));
    pp = QPoint(pp.x() + (this->width() - d->ProgressDialog->width()) / 2,
                pp.y() + (this->height() - d->ProgressDialog->height())/ 2);
    d->ProgressDialog->move(pp - QPoint((targetWidth - d->ProgressDialog->width())/2, 0));
    d->ProgressDialog->resize(targetWidth, d->ProgressDialog->height());
    }
  d->ProgressDialog->setValue( value );
  logger.error(QString("setting value to %1").arg(value) );
  QApplication::processEvents();
}

//----------------------------------------------------------------------------
ctkDICOMTableManager* ctkDICOMQueryRetrieveWidget::dicomTableManager()
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  return d->dicomTableManager;
}

//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQueryRetrieveWidget::getServerParameters()
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  return d->ServerNodeWidget->parameters();
}
