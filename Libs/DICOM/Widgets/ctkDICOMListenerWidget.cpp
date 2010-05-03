
// ctkDICOMWidgets includes
#include "ctkDICOMListenerWidget.h"
#include "ui_ctkDICOMListenerWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMListenerWidgetPrivate: public ctkPrivate<ctkDICOMListenerWidget>,
                                     public Ui_ctkDICOMListenerWidget
{
public:
  ctkDICOMListenerWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMListenerWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMListenerWidget methods

//----------------------------------------------------------------------------
ctkDICOMListenerWidget::ctkDICOMListenerWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMListenerWidget);
  CTK_D(ctkDICOMListenerWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMListenerWidget::~ctkDICOMListenerWidget()
{
}


