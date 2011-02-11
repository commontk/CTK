

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

void ctkDICOMThumbnailWidget::setText(QString &text){
  Q_D(ctkDICOMThumbnailWidget);

  d->textLabel->setText(text);
}

void ctkDICOMThumbnailWidget::setPixmap(QPixmap &pixmap){
  Q_D(ctkDICOMThumbnailWidget);

  d->pixmapLabel->setPixmap(pixmap);
}

void ctkDICOMThumbnailWidget::mousePressEvent(QMouseEvent* event){
  emit selected(*this);
}
