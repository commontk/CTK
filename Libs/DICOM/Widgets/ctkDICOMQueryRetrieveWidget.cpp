#include <QDebug>
#include <QTreeView>
#include <QTabBar>

// ctkDICOMWidgets includes
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMQueryRetrieveWidget.h"

#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMQueryRetrieveWidget");

//----------------------------------------------------------------------------
class ctkDICOMQueryRetrieveWidgetPrivate: public ctkPrivate<ctkDICOMQueryRetrieveWidget>,
                                          public Ui_ctkDICOMQueryRetrieveWidget
{
public:
  ctkDICOMQueryRetrieveWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryRetrieveWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::ctkDICOMQueryRetrieveWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMQueryRetrieveWidget);
  CTK_D(ctkDICOMQueryRetrieveWidget);
  
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
void ctkDICOMQueryRetrieveWidget::onTabCloseRequested(int index)
{
  CTK_D(ctkDICOMQueryRetrieveWidget);

  if (index == 0)
  {
    return;
  }

  d->results->removeTab(index);
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::processQuery()
{
  CTK_D(ctkDICOMQueryRetrieveWidget);

  logger.setDebug();
  logger.debug("initiating query");

  d->serverNodeWidget->populateQuery();
  d->queryWidget->populateQuery();

  QTreeView *queryResults = new QTreeView;
  int tabIndex = d->results->addTab(queryResults, tr("Query Results"));
  d->results->setCurrentIndex(tabIndex);
}

