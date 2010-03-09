
// qCTKDCMTK includes
#include "qCTKDCMTKListenerWidget.h"
#include "ui_qCTKDCMTKListenerWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKListenerWidgetPrivate: public qCTKPrivate<qCTKDCMTKListenerWidget>, 
                                       public Ui_qCTKDCMTKListenerWidget
{
public:
  qCTKDCMTKListenerWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKListenerWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKListenerWidget methods

qCTKDCMTKListenerWidget::qCTKDCMTKListenerWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKListenerWidget);
  QCTK_D(qCTKDCMTKListenerWidget);
  
  d->setupUi(this);
}

qCTKDCMTKListenerWidget::~qCTKDCMTKListenerWidget()
{
}


