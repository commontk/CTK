
// ctkDICOMWidgets includes
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ui_ctkDICOMQueryRetrieveWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMQueryRetrieveWidgetPrivate: public qCTKPrivate<ctkDICOMQueryRetrieveWidget>,
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
  QCTK_INIT_PRIVATE(ctkDICOMQueryRetrieveWidget);
  QCTK_D(ctkDICOMQueryRetrieveWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget::~ctkDICOMQueryRetrieveWidget()
{
}


