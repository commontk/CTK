
// qCTKDCMTK includes
#include "qCTKDCMTKQueryRetrieveWidget.h"
#include "ui_qCTKDCMTKQueryRetrieveWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKQueryRetrieveWidgetPrivate: public qCTKPrivate<qCTKDCMTKQueryRetrieveWidget>, 
                                      public Ui_qCTKDCMTKQueryRetrieveWidget
{
public:
  qCTKDCMTKQueryRetrieveWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKQueryRetrieveWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKQueryRetrieveWidget methods

qCTKDCMTKQueryRetrieveWidget::qCTKDCMTKQueryRetrieveWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKQueryRetrieveWidget);
  QCTK_D(qCTKDCMTKQueryRetrieveWidget);
  
  d->setupUi(this);
}

qCTKDCMTKQueryRetrieveWidget::~qCTKDCMTKQueryRetrieveWidget()
{
}


