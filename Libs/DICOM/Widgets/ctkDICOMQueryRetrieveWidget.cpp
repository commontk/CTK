#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>
#include <QHBoxLayout>

/// CTK includes
#include <ctkCheckableHeaderView.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMQuery.h"
#include "ctkDICOMRetrieve.h"

// ctkDICOMWidgets includes
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMQueryRetrieveWidget.h"


#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMQueryRetrieveWidget");

//----------------------------------------------------------------------------
class ctkDICOMQueryRetrieveWidgetPrivate: public Ui_ctkDICOMQueryRetrieveWidget
{
public:
  ctkDICOMQueryRetrieveWidgetPrivate(){}

  QMap<QString, ctkDICOMQuery*> QueriesByServer;
  QMap<QString, ctkDICOMQuery*> QueriesByStudyUID;
  QMap<QString, ctkDICOMRetrieve*> RetrievalsByStudyUID;
  ctkDICOMDatabase QueryResultDatabase;
  QSharedPointer<ctkDICOMDatabase> RetrieveDatabase;
  ctkDICOMModel model;
};

//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::ctkDICOMQueryRetrieveWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMQueryRetrieveWidgetPrivate)
{
  Q_D(ctkDICOMQueryRetrieveWidget);
  
  d->setupUi(this);

  connect(d->QueryButton, SIGNAL(clicked()), this, SLOT(processQuery()));
  connect(d->RetrieveButton, SIGNAL(clicked()), this, SLOT(processRetrieve()));
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
void ctkDICOMQueryRetrieveWidget::processQuery()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  d->RetrieveButton->setEnabled(false);
  
  // create a database in memory to hold query results
  try { d->QueryResultDatabase.openDatabase( ":memory:", "QUERY-DB" ); }
  catch (std::exception e)
  {
    logger.error ( "Database error: " + d->QueryResultDatabase.lastError() );
    d->QueryResultDatabase.closeDatabase();
    return;
  }

  // for each of the selected server nodes, send the query
  QStringList serverNodes = d->ServerNodeWidget->nodes();
  foreach (QString server, serverNodes)
  {
    QMap<QString, QVariant> parameters = d->ServerNodeWidget->nodeParameters(server);
    if ( parameters["CheckState"] == Qt::Checked )
    {
      // create a query for the current server
      d->QueriesByServer[server] = new ctkDICOMQuery;
      d->QueriesByServer[server]->setCallingAETitle(d->ServerNodeWidget->callingAETitle());
      d->QueriesByServer[server]->setCalledAETitle(parameters["AETitle"].toString());
      d->QueriesByServer[server]->setHost(parameters["Address"].toString());
      d->QueriesByServer[server]->setPort(parameters["Port"].toInt());

      // populate the query with the current search options
      d->QueriesByServer[server]->setFilters( d->QueryWidget->parameters() );

      try
      {
        // run the query against the selected server and put results in database
        d->QueriesByServer[server]->query ( d->QueryResultDatabase );
      }
      catch (std::exception e)
      {
        logger.error ( "Query error: " + parameters["Name"].toString() );
      }

      foreach( QString studyUID, d->QueriesByServer[server]->studyInstanceUIDQueried() )
      {
        d->QueriesByStudyUID[studyUID] = d->QueriesByServer[server];
      }
    }
  }
  
  // checkable headers - allow user to select the patient/studies to retrieve
  d->results->setModel(&d->model);
  d->model.setDatabase(d->QueryResultDatabase.database());

  d->model.setHeaderData(0, Qt::Horizontal, Qt::Unchecked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = d->results->header();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, d->results);
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setPropagateToItems(true);
  d->results->setHeader(headerView);
  // headerView is hidden because it was created with a visisble parent widget 
  headerView->setHidden(false);

  d->RetrieveButton->setEnabled(d->model.rowCount());
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::processRetrieve()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  QMap<QString,QVariant> serverParameters = d->ServerNodeWidget->parameters();

  foreach( QString studyUID, d->QueriesByStudyUID.keys() )
  {
    logger.debug("need to retrieve " + studyUID + " from " + d->QueriesByStudyUID[studyUID]->host());
    ctkDICOMQuery *query = d->QueriesByStudyUID[studyUID];
    ctkDICOMRetrieve *retrieve = new ctkDICOMRetrieve;
    retrieve->setRetrieveDatabase( d->RetrieveDatabase );
    d->RetrievalsByStudyUID[studyUID] = retrieve;
    retrieve->setCallingAETitle( query->callingAETitle() );
    retrieve->setCalledAETitle( query->calledAETitle() );
    retrieve->setCalledPort( query->port() );
    retrieve->setHost( query->host() );

    // pull from GUI
    retrieve->setMoveDestinationAETitle( serverParameters["StorageAETitle"].toString() );
    retrieve->setCallingPort( serverParameters["StoragePort"].toInt() );

    logger.info ( "Starting to retrieve" );
    try
      {
      retrieve->retrieveStudy ( studyUID );
      }
    catch (std::exception e)
      {
      logger.error ( "Retrieve failed" );
      return;
      }
    logger.info ( "Retrieve success" );
  }
}
