
// qCTKDCMTK includes
#include "qCTKDCMTKQueryWidget.h"
#include "ui_qCTKDCMTKQueryWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKQueryWidgetPrivate: public qCTKPrivate<qCTKDCMTKQueryWidget>, 
                                       public Ui_qCTKDCMTKQueryWidget
{
public:
  qCTKDCMTKQueryWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKQueryWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKQueryWidget methods

qCTKDCMTKQueryWidget::qCTKDCMTKQueryWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKQueryWidget);
  QCTK_D(qCTKDCMTKQueryWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
qCTKDCMTKQueryWidget::~qCTKDCMTKQueryWidget()
{
}


