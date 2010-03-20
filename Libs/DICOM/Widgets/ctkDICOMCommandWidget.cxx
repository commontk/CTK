
// ctkDICOMWidgets includes
#include "ctkDICOMCommandWidget.h"
#include "ui_ctkDICOMCommandWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMCommandWidgetPrivate: public qCTKPrivate<ctkDICOMCommandWidget>,
                                    public Ui_ctkDICOMCommandWidget
{
public:
  ctkDICOMCommandWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMCommandWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMCommandWidget methods

//----------------------------------------------------------------------------
ctkDICOMCommandWidget::ctkDICOMCommandWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(ctkDICOMCommandWidget);
  QCTK_D(ctkDICOMCommandWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMCommandWidget::~ctkDICOMCommandWidget()
{
}


