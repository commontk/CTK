
// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMQueryWidgetPrivate: public qCTKPrivate<ctkDICOMQueryWidget>,
                                       public Ui_ctkDICOMQueryWidget
{
public:
  ctkDICOMQueryWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMQueryWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::ctkDICOMQueryWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(ctkDICOMQueryWidget);
  QCTK_D(ctkDICOMQueryWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
}


