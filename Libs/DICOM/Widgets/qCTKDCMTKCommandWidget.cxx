
// qCTKDCMTK includes
#include "qCTKDCMTKCommandWidget.h"
#include "ui_qCTKDCMTKCommandWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKCommandWidgetPrivate: public qCTKPrivate<qCTKDCMTKCommandWidget>, 
                                       public Ui_qCTKDCMTKCommandWidget
{
public:
  qCTKDCMTKCommandWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKCommandWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKCommandWidget methods

qCTKDCMTKCommandWidget::qCTKDCMTKCommandWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKCommandWidget);
  QCTK_D(qCTKDCMTKCommandWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
qCTKDCMTKCommandWidget::~qCTKDCMTKCommandWidget()
{
}


