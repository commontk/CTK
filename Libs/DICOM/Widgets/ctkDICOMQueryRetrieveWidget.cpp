#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>

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

  QMap<QString, ctkDICOMQuery*> queries;
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
  
  ctkDICOMDatabase queryResultDatabase;

  // create a database in memory to hold query results
  try { queryResultDatabase.openDatabase( ":memory:" ); }
  catch (std::exception e)
  {
    logger.error ( "Database error: " + queryResultDatabase.GetLastError() );
    queryResultDatabase.closeDatabase();
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
      d->queries[server] = new ctkDICOMQuery;
      d->queries[server]->setCallingAETitle(d->ServerNodeWidget->callingAETitle());
      d->queries[server]->setCalledAETitle(parameters["AETitle"].toString());
      d->queries[server]->setHost(parameters["Address"].toString());
      d->queries[server]->setPort(parameters["Port"].toInt());

      // populate the query with the current search options
      d->queries[server]->setFilters( d->QueryWidget->parameters() );

      try
      {
        // run the query against the selected server and put results in database
        d->queries[server]->query ( queryResultDatabase );
      }
      catch (std::exception e)
      {
        logger.error ( "Query error: " + parameters["Name"].toString() );
      }
    }
  }

  // checkable headers - allow user to select the patient/studies to retrieve
  d->results->setModel(&d->model);
  d->model.setHeaderData(0, Qt::Horizontal, Qt::Unchecked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = d->results->header();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, d->results);
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setPropagateToItems(true);
  d->results->setHeader(headerView);

  d->model.setDatabase(queryResultDatabase.database());
  d->results->setModel(&d->model);

  if ( d->model.rowCount() > 0 )
  {
    d->RetrieveButton->setEnabled(true);
  }
}
