
// ctkDICOMWidgets includes
#include "ctkDICOMLocationWidget.h"
#include "ui_ctkDICOMLocationWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMLocationWidgetPrivate: public qCTKPrivate<ctkDICOMLocationWidget>,
                                      public Ui_ctkDICOMLocationWidget
{
public:
  ctkDICOMLocationWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMLocationWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMLocationWidget methods

//----------------------------------------------------------------------------
ctkDICOMLocationWidget::ctkDICOMLocationWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(ctkDICOMLocationWidget);
  QCTK_D(ctkDICOMLocationWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMLocationWidget::~ctkDICOMLocationWidget()
{
}


