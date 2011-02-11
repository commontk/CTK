
// Qt include
#include <QGridLayout>
#include <QResizeEvent>
#include <QPushButton>
#include <QPixmap>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailListWidget.h"
#include "ui_ctkDICOMThumbnailListWidget.h"
#include "ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMThumbnailListWidget");

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


}

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::~ctkDICOMThumbnailListWidget()
{
  
}

void ctkDICOMThumbnailListWidget::setThumbnailFiles(const QStringList& thumbnailList)
{
  Q_D(ctkDICOMThumbnailListWidget);

  if (d->scrollAreaContentWidget->layout())
  {
    delete d->scrollAreaContentWidget->layout();
  }

  ctkFlowLayout* flowLayout = new ctkFlowLayout();
  d->scrollAreaContentWidget->setLayout(flowLayout);

  int i = 0;
  foreach (QString thumbnailFile, thumbnailList)
  {
    ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(d->scrollAreaContentWidget);
    QString widgetLabel = QString("Image %1").arg(i++);
    widget->setText( widgetLabel );
    QPixmap pix(thumbnailFile);
    logger.debug("Setting pixmap to " + thumbnailFile);
    widget->setPixmap(pix);
    flowLayout->addWidget(widget);
  }
}
