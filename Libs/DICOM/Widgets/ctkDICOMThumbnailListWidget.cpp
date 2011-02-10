
// Qt include
#include <QGridLayout>
#include <QResizeEvent>
#include <QPushButton>
#include <QPixmap>

// ctkWidgets includes
#include "ctkFlowLayout.h"

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailListWidget.h"
#include "ui_ctkDICOMThumbnailListWidget.h"
#include "ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMThumbnailListWidgetPrivate: public Ui_ctkDICOMThumbnailListWidget
{
public:
  ctkDICOMThumbnailListWidgetPrivate(ctkDICOMThumbnailListWidget* parent): q_ptr(parent){
    
  }

  QList<ctkDICOMThumbnailWidget*> thumbnailList;

  ctkDICOMThumbnailListWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailListWidget);
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::ctkDICOMThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
									   d_ptr(new ctkDICOMThumbnailListWidgetPrivate(this))
{
  Q_D(ctkDICOMThumbnailListWidget);
  
  d->setupUi(this);

  ctkFlowLayout* flowLayout = new ctkFlowLayout();
  d->scrollAreaContentWidget->setLayout(flowLayout);

  for(int i=0; i<326; i++){
    ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(d->scrollAreaContentWidget);

    QString str("thumbnail");
    widget->setText(str);
    QPixmap pix(64, 64);
    pix.fill(Qt::blue);
    widget->setPixmap(pix);
    flowLayout->addWidget(widget);
  }
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::~ctkDICOMThumbnailListWidget()
{
  
}

void ctkDICOMThumbnailListWidget::setModelIndex(const QModelIndex &index){
  Q_D(ctkDICOMThumbnailListWidget);

}
