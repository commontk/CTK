
// qCTKDCMTK includes
#include "qCTKDCMTKLocationWidget.h"
#include "ui_qCTKDCMTKLocationWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKLocationWidgetPrivate: public qCTKPrivate<qCTKDCMTKLocationWidget>, 
                                      public Ui_qCTKDCMTKLocationWidget
{
public:
  qCTKDCMTKLocationWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKLocationWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKLocationWidget methods

qCTKDCMTKLocationWidget::qCTKDCMTKLocationWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKLocationWidget);
  QCTK_D(qCTKDCMTKLocationWidget);
  
  d->setupUi(this);
}

qCTKDCMTKLocationWidget::~qCTKDCMTKLocationWidget()
{
}


