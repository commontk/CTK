
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
  ctkDICOMThumbnailListWidgetPrivate(){}

  void rearrangeThumbnails();

  QList<ctkDICOMThumbnailWidget*> thumbnailList;
  int maxColumnNum;
  int thumbnailWidth;
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
    this->thumbnailLayout->addWidget(this->thumbnailList.at(i), row, column);
    column++;
    if(column >= this->maxColumnNum){
      column = 0;
      row++;
    }
  }
}

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::ctkDICOMThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMThumbnailListWidgetPrivate)
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

void ctkDICOMThumbnailListWidget::clearThumbnail(){
  Q_D(ctkDICOMThumbnailListWidget);
  
  int count = d->thumbnailList.count();

  // clear all thumbnails from layout
  for(int i = 0; i < count; i++){
    d->thumbnailLayout->removeWidget(d->thumbnailList.at(i));
  }

  d->thumbnailList.clear();
}

void ctkDICOMThumbnailListWidget::addThumbnail(ctkDICOMThumbnailWidget* widget){
  Q_D(ctkDICOMThumbnailListWidget);
  if(widget == NULL)return;

  d->thumbnailList.push_back(widget);

  int count = d->thumbnailList.count();

  int lastRow = (count/d->maxColumnNum);
  int lastColumn = (count%d->maxColumnNum);

  int nextRow = (lastRow+((lastColumn+1)/d->maxColumnNum));
  int nextColumn = (lastColumn+1)%d->maxColumnNum;

  d->thumbnailLayout->addWidget(widget, nextRow, nextColumn);
}

void ctkDICOMThumbnailListWidget::addTestThumbnail(){
  Q_D(ctkDICOMThumbnailListWidget);
  for(int i = 0; i<11; i++){
    ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(this);
    QString text("Thumbnail: ");
    widget->setMaximumWidth(d->thumbnailWidth);
    widget->setText(text);
    this->addThumbnail(widget);
  }
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

