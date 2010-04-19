
// ctkDICOMWidgets includes
#include "ctkDICOMListenerWidget.h"
#include "ui_ctkDICOMListenerWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMListenerWidgetPrivate: public qCTKPrivate<ctkDICOMListenerWidget>,
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
  QCTK_INIT_PRIVATE(ctkDICOMListenerWidget);
  QCTK_D(ctkDICOMListenerWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMListenerWidget::~ctkDICOMListenerWidget()
{
}


