/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkQImageView_h
#define __ctkQImageView_h

/// Qt includes
#include <QWidget>
#include <QImage>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkQImageViewPrivate;

/// \ingroup Widgets
///
/// ctkQImageView is the base class of image viewer widgets.
class CTK_WIDGETS_EXPORT ctkQImageView: public QWidget
{

  Q_OBJECT

public:

  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  ctkQImageView( QWidget* parent = 0 );
  
  /// Destructor
  virtual ~ctkQImageView( void );

  double xSpacing( void );
  double ySpacing( void );
  double sliceSpacing( void );
  double sliceThickness( void );

  double xPosition( void );
  double yPosition( void );
  double slicePosition( void );
  double positionValue( void );

  double xCenter( void );
  double yCenter( void );

  int sliceNumber( void ) const;
  int numberOfSlices( void ) const;

  double intensityWindow( void ) const;
  double intensityLevel( void ) const;

  bool invertImage( void ) const;

  bool flipXAxis( void ) const;
  bool flipYAxis( void ) const;
  bool transposeXY( void ) const;

  double zoom( void );

public Q_SLOTS:

  void addImage( const QImage & image );
  void clearImages( void );

  void setSliceNumber( int slicenum );

  void setIntensityWindowLevel( double iwWindow, double iwLevel );

  void setInvertImage( bool invert );
  void setFlipXAxis( bool flip );
  void setFlipYAxis( bool flip );
  void setTransposeXY( bool transpose );

  virtual void keyPressEvent( QKeyEvent * event );
  virtual void mousePressEvent( QMouseEvent * event );
  virtual void mouseReleaseEvent( QMouseEvent * event );
  virtual void mouseMoveEvent( QMouseEvent * event );
  virtual void enterEvent( QEvent * event );
  virtual void leaveEvent( QEvent * event );

  void setCenter( double x, double y );
  void setPosition( double x, double y );

  void setZoom( double factor );

  void reset();

  virtual void update( bool zoomChanged=false, bool sizeChanged=false );

Q_SIGNALS:

  void xSpacingChanged( double xSpacing );
  void ySpacingChanged( double ySpacing );
  void sliceThicknessChanged( double sliceThickness );

  void xPositionChanged( double xPosition );
  void yPositionChanged( double yPosition );
  void slicePositionChanged( double slicePosition );
  void positionValueChanged( double positionValue );

  void sliceNumberChanged( int sliceNum );

  void zoomChanged( double factor );
  void xCenterChanged( double x );
  void yCenterChanged( double y );

  void numberOfSlicesChanged( int numberOfSlices );

  void invertImageChanged( bool invert );

  void flipXAxisChanged( bool flipXAxis );
  void flipYAxisChanged( bool flipYAxis );
  void transposeXYChanged( bool transposeXY );

  void intensityWindowChanged( double intensityWindow );
  void intensityLevelChanged( double intensityLevel );

protected:

  virtual void resizeEvent( QResizeEvent* event );

  /// protected constructor to derive private implementations
  ctkQImageView( ctkQImageViewPrivate & pvt,
    QWidget* parent=0 );

private:

  QScopedPointer< ctkQImageViewPrivate > d_ptr;

  Q_DECLARE_PRIVATE( ctkQImageView );

  Q_DISABLE_COPY( ctkQImageView );

};

#endif
