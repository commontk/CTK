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

  connect(d->queryButton, SIGNAL(clicked()), this, SLOT(processQuery()));
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
  
  ctkDICOMDatabase queryResultDatabase;

  try { queryResultDatabase.openDatabase( ":memory:" ); }
  catch (std::exception e)
  {
    logger.error ( "Database error: " + queryResultDatabase.GetLastError() );
    queryResultDatabase.closeDatabase();
    return;
  }

  QStringList serverNodes = d->serverNodeWidget->nodes();
  foreach (QString server, serverNodes)
  {
    QMap<QString, QVariant> parameters = d->serverNodeWidget->nodeParameters(server);
    if ( parameters["CheckState"] == Qt::Checked )
    {
      d->queries[server] = new ctkDICOMQuery;
      d->queries[server]->setCallingAETitle(d->serverNodeWidget->callingAETitle());
      d->queries[server]->setCalledAETitle(parameters["AETitle"].toString());
      d->queries[server]->setHost(parameters["Address"].toString());
      d->queries[server]->setPort(parameters["Port"].toInt());
      // TODO: add interface to ctkDICOMQuery for specifying query params
      // for now, query for everything

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

  // checkable headers.
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
}
