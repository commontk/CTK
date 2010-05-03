
// ctkDICOMWidgets includes
#include "ctkDICOMLocationWidget.h"
#include "ui_ctkDICOMLocationWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMLocationWidgetPrivate: public ctkPrivate<ctkDICOMLocationWidget>,
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
  CTK_INIT_PRIVATE(ctkDICOMLocationWidget);
  CTK_D(ctkDICOMLocationWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMLocationWidget::~ctkDICOMLocationWidget()
{
}


