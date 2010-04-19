
// ctkDICOMWidgets includes
#include "ctkDICOMServerNodeWidget.h"
#include "ui_ctkDICOMServerNodeWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMServerNodeWidgetPrivate: public qCTKPrivate<ctkDICOMServerNodeWidget>,
                                       public Ui_ctkDICOMServerNodeWidget
{
public:
  ctkDICOMServerNodeWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidget methods

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::ctkDICOMServerNodeWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(ctkDICOMServerNodeWidget);
  QCTK_D(ctkDICOMServerNodeWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::~ctkDICOMServerNodeWidget()
{
}


