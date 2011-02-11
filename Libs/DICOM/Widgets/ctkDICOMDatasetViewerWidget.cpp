/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#include <iostream>

// CTK includes
#include "ctkQImageViewerWidget.h"
#include "ctkDICOMDatasetViewerWidget.h"

// Qt includes
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

//--------------------------------------------------------------------------
class ctkDICOMDatasetViewerWidgetPrivate 
{

  Q_DECLARE_PUBLIC( ctkDICOMDatasetViewerWidget );

protected:

  ctkDICOMDatasetViewerWidget* const q_ptr;

public:

  ctkDICOMDatasetViewerWidgetPrivate( ctkDICOMDatasetViewerWidget& object );

  void init();
};

//--------------------------------------------------------------------------
ctkDICOMDatasetViewerWidgetPrivate::ctkDICOMDatasetViewerWidgetPrivate(
  ctkDICOMDatasetViewerWidget& object )
  : q_ptr( & object )
{
}

//--------------------------------------------------------------------------
void ctkDICOMDatasetViewerWidgetPrivate::init()
{
  /*
  Q_Q( ctkDICOMDatasetViewerWidget );
  this->Window->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Window);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);
  */
}

// -------------------------------------------------------------------------
ctkDICOMDatasetViewerWidget::ctkDICOMDatasetViewerWidget( QWidget* _parent )
  : Superclass( _parent ),
    d_ptr( new ctkDICOMDatasetViewerWidgetPrivate( *this ) )
{
  Q_D( ctkDICOMDatasetViewerWidget );
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetViewerWidget::ctkDICOMDatasetViewerWidget(
  ctkDICOMDatasetViewerWidgetPrivate& pvt,
  QWidget* _parent)
  : Superclass(_parent), d_ptr(&pvt)
{
  Q_D(ctkDICOMDatasetViewerWidget);
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetViewerWidget::~ctkDICOMDatasetViewerWidget()
{
}

// -------------------------------------------------------------------------
//void ctkDICOMDatasetViewerWidget::addImage( const QImage * image )
//{
  //Q_D( ctkQImageViewerWidget );
  //d->ImageList.push_back( image );
  //d->TmpXMin = 0;
  //d->TmpXMax = image->width();
  //d->TmpYMin = 0;
  //d->TmpYMax = image->height();
  //this->update( true, false );
  //this->setCenter( image->width()/2.0, image->height()/2.0 );
//}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewerWidget::mousePressEvent( QMouseEvent * event )
{
  event->ignore();
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewerWidget::mouseMoveEvent( QMouseEvent * event )
{
  event->ignore();
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewerWidget::update( bool zoomChanged,
  bool sizeChanged )
{
  std::cout << "DICOM Updating.." << std::endl;

  Superclass::update( zoomChanged, sizeChanged );
}
