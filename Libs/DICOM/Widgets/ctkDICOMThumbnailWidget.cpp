

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailWidget.h"
#include "ui_ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMThumbnailWidgetPrivate: public Ui_ctkDICOMThumbnailWidget
{
public:
  ctkDICOMThumbnailWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailWidget::ctkDICOMThumbnailWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMThumbnailWidgetPrivate)
{
  Q_D(ctkDICOMThumbnailWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailWidget::~ctkDICOMThumbnailWidget()
{
}

