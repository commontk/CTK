#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>

// ctkDICOMCore includes
#include "ctkDICOM.h"
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

  ctkDICOMQuery query;

  // TODO: convert widget to query parameters
  // TODO: add interface to ctkDICOMQuery for specifying query params
  //d->queryWidget->populateQuery();

  QStringList nodes = d->serverNodeWidget->nodes();
  foreach (QString node, nodes)
  {
    d->queries[node] = new ctkDICOMQuery;
    QMap<QString, QString> parameters = d->serverNodeWidget->nodeParameters(node);
    d->queries[node]->setCallingAETitle(node);
  }

#if 0
TODO: map the server node options to the query classes

  query.setCallingAETitle ( QString ( argv[2] ) );
  query.setCalledAETitle ( QString ( argv[3] ) );
  query.setHost ( QString ( argv[4] ) );
  int port;
  bool ok;
  port = QString ( argv[5] ).toInt ( &ok );
  if ( !ok )
    {
    std::cerr << "Could not convert " << argv[5] << " to an integer" << std::endl;
    print_usage();
    return EXIT_FAILURE;
    }
  query.setPort ( port );

  try
    {
    query.query ( myCTK.database() );
    }
  catch (std::exception e)
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
#endif

  // TODO: create a map of server locations to query results in the private class
  ctkDICOM queryResultDatabase;

  try { queryResultDatabase.openDatabase( ":memory:" ); }
  catch (std::exception e)
  {
    logger.error ( "Database error: " + queryResultDatabase.GetLastError() );
    queryResultDatabase.closeDatabase();
    return;
  }

  ctkDICOMModel model;
  model.setDatabase(queryResultDatabase.database());
  d->results->setModel(&model);
}
