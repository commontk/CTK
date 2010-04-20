
// ctkDICOMWidgets includes
#include "ctkDICOMCommandWidget.h"
#include "ui_ctkDICOMCommandWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMCommandWidgetPrivate: public ctkPrivate<ctkDICOMCommandWidget>,
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
  CTK_INIT_PRIVATE(ctkDICOMCommandWidget);
  CTK_D(ctkDICOMCommandWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMCommandWidget::~ctkDICOMCommandWidget()
{
}


