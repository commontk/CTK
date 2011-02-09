
// Qt include
#include <QGridLayout>
#include <QResizeEvent>

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

  void clearAllThumbnails();
  void addThumbnail(ctkDICOMThumbnailWidget* widget);
  void rearrangeThumbnails();

  QList<ctkDICOMThumbnailWidget*> thumbnailList;
  int maxColumnNum;
  int thumbnailWidth;

  ctkDICOMThumbnailListWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailListWidget);
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidgetPrivate methods

void ctkDICOMThumbnailListWidgetPrivate::rearrangeThumbnails(){
  int count = this->thumbnailList.count();

  // clear all thumbnails from layout
  for(int i = 0; i < count; i++){
    this->thumbnailLayout->removeWidget(this->thumbnailList.at(i));
  }

  // add all thumbnails to layout with new arrangement
  int row = 0;
  int column = 0;
  for(int i = 0; i < count; i++){
    ctkDICOMThumbnailWidget* widget = this->thumbnailList.at(i);
    this->thumbnailLayout->addWidget(widget, row, column);
    column++;
    if(column >= this->maxColumnNum){
      column = 0;
      row++;
    }
  }
}

void ctkDICOMThumbnailListWidgetPrivate::addThumbnail(ctkDICOMThumbnailWidget* widget){
  Q_Q(ctkDICOMThumbnailListWidget);

  if(widget == NULL)return;

  this->thumbnailList.push_back(widget);

  q->connect(widget, SIGNAL(selected(const ctkDICOMThumbnailWidget&)), q,  SIGNAL(selected(const ctkDICOMThumbnailWidget&)));
}

void ctkDICOMThumbnailListWidgetPrivate::clearAllThumbnails(){
  int count = this->thumbnailList.count();

  // clear all thumbnails from layout
  for(int i = 0; i < count; i++){
    QWidget* widget = this->thumbnailList.at(i);
    this->thumbnailLayout->removeWidget(widget);
    widget->deleteLater();
  }

  this->thumbnailList.clear();
}

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::ctkDICOMThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
									   d_ptr(new ctkDICOMThumbnailListWidgetPrivate(this))
{
  Q_D(ctkDICOMThumbnailListWidget);
  
  d->setupUi(this);
  d->maxColumnNum = 4;
  d->thumbnailWidth = 128;
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::~ctkDICOMThumbnailListWidget()
{
}

void ctkDICOMThumbnailListWidget::addTestThumbnail(int count){
  Q_D(ctkDICOMThumbnailListWidget);
  for(int i = 0; i<count; i++){
    ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(this);
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::blue);
    QString text("Thumbnail: ");
    widget->setMaximumWidth(d->thumbnailWidth);
    widget->setText(text);
    widget->setPixmap(pixmap);
    d->addThumbnail(widget);
  }
  d->rearrangeThumbnails();
}

void ctkDICOMThumbnailListWidget::setThumbnailWidth(int width){
  Q_D(ctkDICOMThumbnailListWidget);
  
  d->thumbnailWidth = width;

  d->maxColumnNum = this->width()/d->thumbnailWidth;
  d->rearrangeThumbnails();
}

void ctkDICOMThumbnailListWidget::resizeEvent ( QResizeEvent * event ){
  Q_D(ctkDICOMThumbnailListWidget);
  
  d->maxColumnNum = event->size().width()/d->thumbnailWidth;
  d->rearrangeThumbnails();
}

void ctkDICOMThumbnailListWidget::setModelIndex(const QModelIndex &index){
  Q_D(ctkDICOMThumbnailListWidget);

  d->clearAllThumbnails();
  this->addTestThumbnail(index.model()->rowCount(index));
  
}
