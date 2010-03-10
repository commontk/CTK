
// qCTKDCMTK includes
#include "qCTKDCMTKServerNodeWidget.h"
#include "ui_qCTKDCMTKServerNodeWidget.h"

//----------------------------------------------------------------------------
class qCTKDCMTKServerNodeWidgetPrivate: public qCTKPrivate<qCTKDCMTKServerNodeWidget>, 
                                       public Ui_qCTKDCMTKServerNodeWidget
{
public:
  qCTKDCMTKServerNodeWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKServerNodeWidgetPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKServerNodeWidget methods

qCTKDCMTKServerNodeWidget::qCTKDCMTKServerNodeWidget(QWidget* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKServerNodeWidget);
  QCTK_D(qCTKDCMTKServerNodeWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
qCTKDCMTKServerNodeWidget::~qCTKDCMTKServerNodeWidget()
{
}


