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
#include "ctkQImageView.h"
#include "ctkDICOMDatasetView.h"

// Qt includes
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

//--------------------------------------------------------------------------
class ctkDICOMDatasetViewPrivate 
{

  Q_DECLARE_PUBLIC( ctkDICOMDatasetView );

protected:

  ctkDICOMDatasetView* const q_ptr;

public:

  ctkDICOMDatasetViewPrivate( ctkDICOMDatasetView& object );

  void init();
};

//--------------------------------------------------------------------------
ctkDICOMDatasetViewPrivate::ctkDICOMDatasetViewPrivate(
  ctkDICOMDatasetView& object )
  : q_ptr( & object )
{
}

//--------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::init()
{
  /*
  Q_Q( ctkDICOMDatasetView );
  this->Window->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Window);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);
  */
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::ctkDICOMDatasetView( QWidget* _parent )
  : Superclass( _parent ),
    d_ptr( new ctkDICOMDatasetViewPrivate( *this ) )
{
  Q_D( ctkDICOMDatasetView );
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::ctkDICOMDatasetView(
  ctkDICOMDatasetViewPrivate& pvt,
  QWidget* _parent)
  : Superclass(_parent), d_ptr(&pvt)
{
  Q_D(ctkDICOMDatasetView);
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::~ctkDICOMDatasetView()
{
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::addImage( const ctkDICOMImage & image )
{
  for( unsigned int i=0; i<image.frameCount(); ++i )
    {
    Superclass::addImage( image.frame( i ) );
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::addImage( const QImage & image )
{
  Superclass::addImage( image );
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::update( bool zoomChanged,
  bool sizeChanged )
{
  Superclass::update( zoomChanged, sizeChanged );
}
