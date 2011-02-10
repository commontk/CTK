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

  QMap<QString, ctkDICOMQuery*> queriesByServer;
  QMap<QString, ctkDICOMQuery*> queriesByStudyUID;
  QMap<QString, ctkDICOMRetrieve*> retrievalsByStudyUID;
  ctkDICOMDatabase queryResultDatabase;
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
void ctkDICOMQueryRetrieveWidget::setRetrieveDirectory(const QString& directory)
{
  QSettings settings;
  settings.setValue("RetrieveDirectory", directory);
  settings.sync();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::setRetrieveDatabaseFileName(const QString& fileName)
{
  QSettings settings;
  settings.setValue("RetrieveDatabaseFileName", fileName);
  settings.sync();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::processQuery()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  d->RetrieveButton->setEnabled(false);
  
  // create a database in memory to hold query results
  try { d->queryResultDatabase.openDatabase( ":memory:" ); }
  catch (std::exception e)
  {
    logger.error ( "Database error: " + d->queryResultDatabase.GetLastError() );
    d->queryResultDatabase.closeDatabase();
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
      d->queriesByServer[server] = new ctkDICOMQuery;
      d->queriesByServer[server]->setCallingAETitle(d->ServerNodeWidget->callingAETitle());
      d->queriesByServer[server]->setCalledAETitle(parameters["AETitle"].toString());
      d->queriesByServer[server]->setHost(parameters["Address"].toString());
      d->queriesByServer[server]->setPort(parameters["Port"].toInt());

      // populate the query with the current search options
      d->queriesByServer[server]->setFilters( d->QueryWidget->parameters() );

      try
      {
        // run the query against the selected server and put results in database
        d->queriesByServer[server]->query ( d->queryResultDatabase );
      }
      catch (std::exception e)
      {
        logger.error ( "Query error: " + parameters["Name"].toString() );
      }

      foreach( QString studyUID, d->queriesByServer[server]->studyInstanceUIDQueried() )
      {
        d->queriesByStudyUID[studyUID] = d->queriesByServer[server];
      }
    }
  }
  
  // checkable headers - allow user to select the patient/studies to retrieve
  d->results->setModel(&d->model);
  d->model.setDatabase(d->queryResultDatabase.database());

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

  foreach( QString studyUID, d->queriesByStudyUID.keys() )
  {
    logger.debug("need to retrieve " + studyUID + " from " + d->queriesByStudyUID[studyUID]->host());
    ctkDICOMQuery *query = d->queriesByStudyUID[studyUID];
    ctkDICOMRetrieve *retrieve = new ctkDICOMRetrieve;
    d->retrievalsByStudyUID[studyUID] = retrieve;
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
      retrieve->retrieveStudy ( studyUID, QDir("/tmp/ctk") );
      }
    catch (std::exception e)
      {
      logger.error ( "Retrieve failed" );
      return;
      }
    logger.info ( "Retrieve success" );
  }
}
