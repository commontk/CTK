
// ctkDICOMWidgets includes
#include "ctkDICOMListenerWidget.h"
#include "ui_ctkDICOMListenerWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMListenerWidgetPrivate: public Ui_ctkDICOMListenerWidget
{
public:
  ctkDICOMListenerWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMListenerWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMListenerWidget methods

//----------------------------------------------------------------------------
ctkDICOMListenerWidget::ctkDICOMListenerWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMListenerWidgetPrivate)
{
  Q_D(ctkDICOMListenerWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMListenerWidget::~ctkDICOMListenerWidget()
{
}


