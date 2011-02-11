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

// Qt includes
#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

//--------------------------------------------------------------------------
class ctkQImageViewPrivate
{

  Q_DECLARE_PUBLIC( ctkQImageView );

protected:

  ctkQImageView* const q_ptr;

public:

  ctkQImageViewPrivate( ctkQImageView& object );

  void init();

  QLabel * Window;

  double Zoom;
  double PositionX;
  double PositionY;
  double CenterX;
  double CenterY;
  int    SliceNumber;

  double IntensityWindowMin;
  double IntensityWindowMax;

  bool FlipXAxis;
  bool FlipYAxis;
  bool TransposeXY;

  QList< QImage > ImageList;

  QPixmap TmpImage;
  int     TmpXMin;
  int     TmpXMax;
  int     TmpYMin;
  int     TmpYMax;

  int    MouseLastX;
  int    MouseLastY;
  double MouseLastZoom;
  double MouseLastIntensityWindowMin;
  double MouseLastIntensityWindowMax;
  bool   MouseLeftDragging;
  bool   MouseMiddleDragging;
  bool   MouseRightDragging;

  double clamp( double x, double xMin, double xMax );

  void fitImageRectangle( double x0, double y0, double x1, double y1 );
  
};

//--------------------------------------------------------------------------
ctkQImageViewPrivate::ctkQImageViewPrivate(
  ctkQImageView& object )
  : q_ptr( &object )
{
  this->Window = new QLabel();
}

//--------------------------------------------------------------------------
void ctkQImageViewPrivate::init()
{
  Q_Q( ctkQImageView );

  this->Window->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Window);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);


  // Set parameters for the view
  this->Zoom = 1;
  this->PositionX = 0;
  this->PositionY = 0;
  this->SliceNumber = 0;

  this->CenterX = 0;
  this->CenterY = 0;

  this->IntensityWindowMin = 0;
  this->IntensityWindowMax = 0;

  this->FlipXAxis = false;
  this->FlipYAxis = true;
  this->TransposeXY = false;

  this->ImageList.clear();

  this->TmpXMin = 0;
  this->TmpXMax = 0;
  this->TmpYMin = 0;
  this->TmpYMax = 0;

  this->MouseLastX = 0;
  this->MouseLastY = 0;
  this->MouseLastZoom = 0;
  this->MouseLeftDragging = false;
  this->MouseMiddleDragging = false;
  this->MouseRightDragging = false;

  // Don't expand for no reason
  q->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
}

//--------------------------------------------------------------------------
double ctkQImageViewPrivate::clamp( double x, double xMin,
  double xMax )
{
  if( x < xMin )
    {
    return xMin;
    }
  if( x > xMax )
    {
    return xMax;
    }
  return x;
}

//--------------------------------------------------------------------------
void ctkQImageViewPrivate::fitImageRectangle( double x0,
  double x1, double y0, double y1 )
{
  if( this->SliceNumber >= 0 && this->SliceNumber < this->ImageList.size() )
    {
    this->TmpXMin = this->clamp( x0, 0,
      this->ImageList[ this->SliceNumber ].width() );
    this->TmpXMax = this->clamp( x1, this->TmpXMin,
      this->ImageList[ this->SliceNumber ].width() );
    this->TmpYMin = this->clamp( y0, 0,
      this->ImageList[ this->SliceNumber ].height() );
    this->TmpYMax = this->clamp( y1, this->TmpYMin,
      this->ImageList[ this->SliceNumber ].height() );

    this->CenterX = ( this->TmpXMax + this->TmpXMin ) / 2.0;
    this->CenterY = ( this->TmpYMax + this->TmpYMin ) / 2.0;
    }
}


// -------------------------------------------------------------------------
ctkQImageView::ctkQImageView( QWidget* _parent )
  : Superclass( _parent ),
    d_ptr( new ctkQImageViewPrivate( *this ) )
{
  Q_D( ctkQImageView );
  d->init();
  d->TmpXMax = this->width();
  d->TmpYMax = this->height();
}

// -------------------------------------------------------------------------
ctkQImageView::ctkQImageView(
  ctkQImageViewPrivate& pvt,
  QWidget* _parent)
  : Superclass(_parent), d_ptr(&pvt)
{
  Q_D(ctkQImageView);
  d->init();
}

// -------------------------------------------------------------------------
ctkQImageView::~ctkQImageView()
{
}

// -------------------------------------------------------------------------
void ctkQImageView::addImage( const QImage & image )
{
  Q_D( ctkQImageView );
  d->ImageList.push_back( image );
  d->TmpXMin = 0;
  d->TmpXMax = image.width();
  d->TmpYMin = 0;
  d->TmpYMax = image.height();
  this->update( true, false );
  this->setCenter( image.width()/2.0, image.height()/2.0 );
}

// -------------------------------------------------------------------------
void ctkQImageView::clearImages( void )
{
  Q_D( ctkQImageView );
  d->ImageList.clear();
  this->update( true, true );
}

// -------------------------------------------------------------------------
double ctkQImageView::xSpacing( void )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    return( 1000.0 / d->ImageList[ d->SliceNumber ].dotsPerMeterX() );
    }
  else
    {
    return 1;
    }
}

// -------------------------------------------------------------------------
double ctkQImageView::ySpacing( void )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    return( 1000.0 / d->ImageList[ d->SliceNumber ].dotsPerMeterY() );
    }
  else
    {
    return 1;
    }
}

// -------------------------------------------------------------------------
double ctkQImageView::sliceThickness( void )
{
  return 1;
}

// -------------------------------------------------------------------------
double ctkQImageView::xPosition( void )
{
  Q_D( ctkQImageView );
  return d->PositionX;
}

// -------------------------------------------------------------------------
double ctkQImageView::yPosition( void )
{
  Q_D( ctkQImageView );
  return d->PositionY;
}

// -------------------------------------------------------------------------
double ctkQImageView::slicePosition( void )
{
  Q_D( ctkQImageView );
  return d->SliceNumber;
}

// -------------------------------------------------------------------------
double ctkQImageView::positionValue( void )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    QColor vc( d->ImageList[ d->SliceNumber ].pixel( d->PositionX,
      d->PositionY ) );
    return vc.value();
    }
  return 0;
}

// -------------------------------------------------------------------------
double ctkQImageView::xCenter( void )
{
  Q_D( ctkQImageView );
  return d->CenterX;
}

// -------------------------------------------------------------------------
double ctkQImageView::yCenter( void )
{
  Q_D( ctkQImageView );
  return d->CenterY;
}

// -------------------------------------------------------------------------
void ctkQImageView::setSliceNumber( int slicenum )
{
  Q_D( ctkQImageView );
  if( slicenum >= 0 && slicenum < d->ImageList.size() 
    && slicenum != d->SliceNumber )
    {
    d->SliceNumber = slicenum;
    emit this->sliceNumberChanged( slicenum );
    emit this->xSpacingChanged( this->xSpacing() );
    emit this->ySpacingChanged( this->ySpacing() );
    emit this->sliceThicknessChanged( this->sliceThickness() );
    emit this->slicePositionChanged( this->slicePosition() );
    this->update( false, false );
    }
}
//
// -------------------------------------------------------------------------
int ctkQImageView::sliceNumber( void ) const
{
  Q_D( const ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    return d->SliceNumber;
    }
  else
    {
    return -1;
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::setIntensityWindow( double iwMin, double iwMax )
{
  Q_D( ctkQImageView );
  if( iwMin != d->IntensityWindowMin )
    {
    d->IntensityWindowMin = iwMin;
    d->IntensityWindowMax = iwMax;
    emit this->intensityWindowMinChanged( iwMin );
    emit this->intensityWindowMaxChanged( iwMax );
    this->update( false, false );
    }
}

// -------------------------------------------------------------------------
double ctkQImageView::intensityWindowMin( void ) const
{
  Q_D( const ctkQImageView );
  return d->IntensityWindowMin;
}

// -------------------------------------------------------------------------
double ctkQImageView::intensityWindowMax( void ) const
{
  Q_D( const ctkQImageView );
  return d->IntensityWindowMax;
}

// -------------------------------------------------------------------------
void ctkQImageView::setFlipXAxis( bool flip )
{
  Q_D( ctkQImageView );
  if( flip != d->FlipXAxis )
    {
    d->FlipXAxis = flip;
    emit this->flipXAxisChanged( flip );
    this->update( false, false );
    }
}

// -------------------------------------------------------------------------
bool ctkQImageView::flipXAxis( void ) const
{
  Q_D( const ctkQImageView );
  return d->FlipXAxis;
}

// -------------------------------------------------------------------------
void ctkQImageView::setFlipYAxis( bool flip )
{
  Q_D( ctkQImageView );
  if( flip != d->FlipYAxis )
    {
    d->FlipYAxis = flip;
    emit this->flipYAxisChanged( flip );
    this->update( false, false );
    }
}

// -------------------------------------------------------------------------
bool ctkQImageView::flipYAxis( void ) const
{
  Q_D( const ctkQImageView );
  return d->FlipYAxis;
}


// -------------------------------------------------------------------------
void ctkQImageView::setTransposeXY( bool transpose )
{
  Q_D( ctkQImageView );
  if( transpose != d->TransposeXY )
    {
    d->TransposeXY = transpose;
    emit this->transposeXYChanged( transpose );
    this->update( false, false );
    }
}

// -------------------------------------------------------------------------
bool ctkQImageView::transposeXY( void ) const
{
  Q_D( const ctkQImageView );
  return d->TransposeXY;
}

// -------------------------------------------------------------------------
void ctkQImageView::setCenter( double x, double y )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
	  int tmpXRange = d->TmpXMax - d->TmpXMin;
    if( tmpXRange > d->ImageList[ d->SliceNumber ].width() )
      {
      tmpXRange = d->ImageList[ d->SliceNumber ].width();
      }
    int tmpYRange = d->TmpYMax - d->TmpYMin;
    if( tmpYRange > d->ImageList[ d->SliceNumber ].height() )
      {
      tmpYRange = d->ImageList[ d->SliceNumber ].height();
      }
  
    int xMin2 = static_cast<int>(x) - tmpXRange/2.0;
    if( xMin2 < 0 )
      {
      xMin2 = 0;
      }
    int xMax2 = xMin2 + tmpXRange;
    if( xMax2 > d->ImageList[ d->SliceNumber ].width() )
      {
      xMax2 = d->ImageList[ d->SliceNumber ].width();
      xMin2 = xMax2 - tmpXRange;
      }
    int yMin2 = static_cast<int>(y) - tmpYRange/2.0;
    if( yMin2 < 0 )
      {
      yMin2 = 0;
      }
    int yMax2 = yMin2 + tmpYRange;
    if( yMax2 > d->ImageList[ d->SliceNumber ].height() )
      {
      yMax2 = d->ImageList[ d->SliceNumber ].height();
      yMin2 = yMax2 - tmpYRange;
      }
    d->fitImageRectangle( xMin2, xMax2, yMin2, yMax2 );

    emit this->xCenterChanged( x );
    emit this->yCenterChanged( y );
  
	  this->update( false, false );
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::setPosition( double x, double y )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    d->PositionX = x;
    d->PositionY = y;

    emit this->xPositionChanged( x );
    emit this->yPositionChanged( y );
    emit this->positionValueChanged( this->positionValue() );

	  this->update( false, false );
    }
}

// -------------------------------------------------------------------------
double ctkQImageView::zoom( void )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    return d->Zoom;
    }
  return 1;
}

// -------------------------------------------------------------------------
void ctkQImageView::setZoom( double factor )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    const QImage * img = & d->ImageList[ d->SliceNumber ];
    if( factor < 2.0 / img->width() )
      {
      factor = 2.0 / img->width();
      }
    if( factor > img->width()/2.0 )
      {
      factor = img->width()/2.0;
      }
    d->Zoom = factor;

    double cx = ( d->TmpXMin + d->TmpXMax ) / 2.0;
    double cy = ( d->TmpYMin + d->TmpYMax ) / 2.0;
    double x2 = img->width() / factor;
    double y2 = img->height() / factor;
    //double x2 = ( d->TmpXMax - d->TmpXMin ) / 2.0;
    //double y2 = ( d->TmpYMax - d->TmpYMin ) / 2.0; 
	  
    int xMin2 = static_cast<int>(cx) - x2 / 2.0;
    if( xMin2 < 0 )
      {
      xMin2 = 0;
      }
    int xMax2 = xMin2 + x2;
    if( xMax2 > d->ImageList[ d->SliceNumber ].width() )
      {
      xMax2 = d->ImageList[ d->SliceNumber ].width();
      xMin2 = xMax2 - x2;
      }
    int yMin2 = static_cast<int>(cy) - y2 / 2.0;
    if( yMin2 < 0 )
      {
      yMin2 = 0;
      }
    int yMax2 = yMin2 + y2;
    if( yMax2 > d->ImageList[ d->SliceNumber ].height() )
      {
      yMax2 = d->ImageList[ d->SliceNumber ].height();
      yMin2 = yMax2 - y2;
      }
    d->fitImageRectangle( xMin2, xMax2, yMin2, yMax2 );
  
	  this->update( true, true );
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::reset( )
{
  Q_D( ctkQImageView );

  if( d->ImageList.size() > 0 )
    {
    if( d->SliceNumber < 0 )
      {
      this->setSliceNumber( 0 );
      }
    }

  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    d->fitImageRectangle( 0, 0, d->ImageList[ d->SliceNumber ].width(),
      d->ImageList[ d->SliceNumber ].height() );

	  this->update( true, true );
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::keyPressEvent( QKeyEvent * event )
{
  Q_D( ctkQImageView );

  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    switch( event->key() )
      {
      case Qt::Key_R:
        {
        this->reset();
        break;
        }
      case Qt::Key_Up:
        {
        this->setSliceNumber( d->SliceNumber+1 );
        break;
        }
      case Qt::Key_Down:
        {
        this->setSliceNumber( d->SliceNumber-1 );
        break;
        }
      default:
        {
        event->ignore();
        }
      };
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::mousePressEvent( QMouseEvent * event )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    switch( event->button() )
      {
      case Qt::LeftButton:
        {
        d->MouseLeftDragging = true;
        d->MouseLastX = event->x();
        d->MouseLastY = event->y();
        d->MouseLastIntensityWindowMin = this->intensityWindowMin();
        d->MouseLastIntensityWindowMax = this->intensityWindowMax();
        break;
        }
      case Qt::MidButton:
        {
        d->MouseMiddleDragging = true;
        d->MouseLastX = event->x();
        d->MouseLastY = event->y();
        d->MouseLastZoom = this->zoom();
        break;
        }
      case Qt::RightButton:
        {
        d->MouseRightDragging = true;
        double relativeX = static_cast<double>( event->x() ) 
          / this->width();
        double relativeY = static_cast<double>( event->y() ) 
          / this->height();
        double x = (d->TmpXMax - d->TmpXMin) * relativeX + d->TmpXMin;
        double y = (d->TmpYMax - d->TmpYMin) * relativeY + d->TmpYMin;
        this->setCenter( x, y );
        break;
        }
      default:
        {
        event->ignore();
        }
      };
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::mouseReleaseEvent( QMouseEvent * event )
{
  Q_D( ctkQImageView );
  d->MouseLeftDragging = false;
  d->MouseMiddleDragging = false;
  d->MouseRightDragging = false;
  event->ignore();
}

// -------------------------------------------------------------------------
void ctkQImageView::mouseMoveEvent( QMouseEvent * event )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    if( d->MouseLeftDragging )
      {
      double distX = d->MouseLastX - event->x();
      double distY = d->MouseLastY - event->y();
      double deltaMin = ( distX / this->height() );
      if( deltaMin < 0 )  
        {
        // Heuristic to make shrinking propotional to enlarging
        deltaMin *= -deltaMin;
        }
      double deltaMax = ( distY / this->width() );
      if( deltaMax < 0 )  
        {
        // Heuristic to make shrinking propotional to enlarging
        deltaMax *= -deltaMax;
        }
      double iRange = d->MouseLastIntensityWindowMax 
        - d->MouseLastIntensityWindowMin;
      deltaMin *= iRange;
      deltaMax *= iRange;
      double newMin = d->MouseLastIntensityWindowMin + deltaMin;
      double newMax = d->MouseLastIntensityWindowMax + deltaMax;
      this->setIntensityWindow( newMin, newMax );
      }
    else if( d->MouseMiddleDragging )
      {
      double distY = d->MouseLastY - event->y();
      double deltaZ = (distY / this->height());
      if( deltaZ < 0 )  
        {
        // Heuristic to make shrinking propotional to enlarging
        deltaZ *= -deltaZ;
        }
      double newZoom = d->MouseLastZoom + deltaZ;
      this->setZoom( newZoom );
      }
    else
      {
      double relativeX = static_cast<double>( event->x() ) 
        / this->width();
      double relativeY = static_cast<double>( event->y() ) 
        / this->height();
      double x = (d->TmpXMax - d->TmpXMin) * relativeX + d->TmpXMin;
      double y = (d->TmpYMax - d->TmpYMin) * relativeY + d->TmpYMin;
      this->setPosition( x, y );
      }
    }
}

// -------------------------------------------------------------------------
void ctkQImageView::enterEvent( QEvent * )
{
  Q_D( ctkQImageView );
  QApplication::setOverrideCursor( QCursor(Qt::CrossCursor) );
  d->Window->grabKeyboard();
}

// -------------------------------------------------------------------------
void ctkQImageView::leaveEvent( QEvent * )
{
  Q_D( ctkQImageView );
  QApplication::restoreOverrideCursor();
  d->Window->releaseKeyboard();
}

// -------------------------------------------------------------------------
void ctkQImageView::resizeEvent( QResizeEvent* event )
{
  this->Superclass::resizeEvent( event );
  this->update( false, true );
}

// -------------------------------------------------------------------------
void ctkQImageView::update( bool zoomChanged,
  bool sizeChanged )
{
  Q_D( ctkQImageView );
  if( d->SliceNumber >= 0 && d->SliceNumber < d->ImageList.size() )
    {
    const QImage * img = & ( d->ImageList[ d->SliceNumber ] );
    if( zoomChanged || sizeChanged )
      {
      if( this->width() > 0 &&  this->height() > 0 
        && d->TmpXMax > d->TmpXMin && d->TmpYMax > d->TmpYMin)
        {
        int tmpXRange = d->TmpXMax - d->TmpXMin;
        int tmpYRange = d->TmpYMax - d->TmpYMin;
        double tmpAspectRatio = static_cast<double>(tmpYRange) / tmpXRange;
        double screenAspectRatio = static_cast<double>(this->height())
          / this->width();
        if( screenAspectRatio > tmpAspectRatio )
          {
          int extraTmpYAbove = d->TmpYMin;
          int extraTmpYBelow = img->height() - d->TmpYMax;
          int extraTmpYNeeded = tmpXRange * screenAspectRatio 
            - tmpYRange;
          int minExtra = extraTmpYAbove;
          if( extraTmpYBelow < minExtra )
            {
            minExtra = extraTmpYBelow;
            }
          if(2 * minExtra >= extraTmpYNeeded)
            {
            int minNeeded = extraTmpYNeeded / 2.0;
            int maxNeeded = extraTmpYNeeded - minNeeded;
            d->TmpYMin -= minNeeded;
            d->TmpYMax += maxNeeded;
            }
          else if(extraTmpYAbove + extraTmpYBelow >= extraTmpYNeeded)
            {
            if(extraTmpYAbove < extraTmpYBelow)
              {
              d->TmpYMin = 0;
              d->TmpYMax += extraTmpYNeeded - extraTmpYAbove;
              }
            else
              {
              d->TmpYMax = img->height();
              d->TmpYMin -= extraTmpYNeeded - extraTmpYBelow;
              }
            }
          else
            {
            d->TmpYMin = 0;
            d->TmpYMax = img->height();
            }
          d->TmpImage = QPixmap( this->width(),
            static_cast<unsigned int>( 
              static_cast<double>(d->TmpYMax - d->TmpYMin) 
              / (d->TmpXMax - d->TmpXMin)
              * this->width() + 0.5 ) );
          }
        else if(screenAspectRatio < tmpAspectRatio)
          {
          int extraTmpXLeft = d->TmpXMin;
          int extraTmpXRight = img->width() - d->TmpXMax;
          int extraTmpXNeeded = static_cast<double>(tmpYRange) 
            / screenAspectRatio - tmpXRange;
          int minExtra = extraTmpXLeft;
          if( extraTmpXRight < minExtra )
            {
            minExtra = extraTmpXRight;
            }
          if(2 * minExtra >= extraTmpXNeeded)
            {
            int minNeeded = extraTmpXNeeded / 2.0;
            int maxNeeded = extraTmpXNeeded - minNeeded;
            d->TmpXMin -= minNeeded;
            d->TmpXMax += maxNeeded;
            }
          else if(extraTmpXLeft + extraTmpXRight >= extraTmpXNeeded)
            {
            if(extraTmpXLeft < extraTmpXRight)
              {
              d->TmpXMin = 0;
              d->TmpXMax += extraTmpXNeeded - extraTmpXLeft;
              }
            else
              {
              d->TmpXMax = img->width();
              d->TmpXMin -= extraTmpXNeeded - extraTmpXRight;
              }
            }
           else
            {
            d->TmpXMin = 0;
            d->TmpXMax = img->width();
            }
          d->TmpImage = QPixmap( static_cast<unsigned int>( this->height()
            / ( static_cast<double>(d->TmpYMax - d->TmpYMin) 
            / (d->TmpXMax - d->TmpXMin) ) 
            + 0.5 ), this->height() );
          }
        else
          {
          d->TmpImage = QPixmap( this->width(),  this->height() );
          }
        }
      }

    if( d->TmpImage.width() > 0 &&  d->TmpImage.height() > 0)
      {
      QPainter painter( &(d->TmpImage) );
      painter.drawPixmap( 0, 0, d->TmpImage.width(), d->TmpImage.height(),
        QPixmap::fromImage(*img), d->TmpXMin, d->TmpYMin,
        d->TmpXMax - d->TmpXMin, d->TmpYMax - d->TmpYMin);
      }

    d->Window->setPixmap( d->TmpImage );
    }
  else
    {
    d->Window->setText( "No Image Loaded." );
    }
}
