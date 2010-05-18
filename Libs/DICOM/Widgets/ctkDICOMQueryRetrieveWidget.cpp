
#include <QTreeView>

// ctkDICOMWidgets includes
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ui_ctkDICOMQueryRetrieveWidget.h"

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

  connect(d->queryButton, SIGNAL(clicked()), this, SLOT(processQuery()));
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::~ctkDICOMQueryRetrieveWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMQueryRetrieveWidget::processQuery()
{
  CTK_D(ctkDICOMQueryRetrieveWidget);

  QTreeView *queryResults = new QTreeView;
  d->results->addTab(queryResults, "Query Results");
}

