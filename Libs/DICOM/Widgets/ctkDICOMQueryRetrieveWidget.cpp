#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>

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

  d->results->disableCloseOnTab(0);
  connect(d->queryButton, SIGNAL(clicked()), this, SLOT(processQuery()));
  connect(d->results, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::~ctkDICOMQueryRetrieveWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::onDatabaseDirectoryChanged(const QString& directory)
{
  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::onTabCloseRequested(int index)
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  if (index == 0)
  {
    return;
  }

  d->results->removeTab(index);
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::processQuery()
{
  Q_D(ctkDICOMQueryRetrieveWidget);

  d->serverNodeWidget->populateQuery();
  d->queryWidget->populateQuery();

  QTreeView *queryResults = new QTreeView;
  int tabIndex = d->results->addTab(queryResults, tr("Query Results"));
  d->results->setCurrentIndex(tabIndex);
}

