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

#ifndef __ctkDICOMDatasetViewerWidget_h
#define __ctkDICOMDatasetViewerWidget_h

/// Qt includes
#include <QWidget>
#include <QImage>

/// CTK includes
#include "ctkQImageViewerWidget.h"
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkDICOMDatasetViewerWidgetPrivate;

///
/// ctkDICOMDatasetViewerWidget is the base class of image viewer widgets.
class CTK_WIDGETS_EXPORT ctkDICOMDatasetViewerWidget
: public ctkQImageViewerWidget
{

  Q_OBJECT

public:

  /// Superclass typedef
  typedef ctkQImageViewerWidget Superclass;

  /// Constructor
  ctkDICOMDatasetViewerWidget( QWidget * parent = 0 );
  
  /// Destructor
  virtual ~ctkDICOMDatasetViewerWidget( void );

public slots:

  //void add( const QImage * image );

  void mousePressEvent( QMouseEvent * event );
  void mouseMoveEvent( QMouseEvent * event );

  virtual void update( bool zoomChanged=false, bool sizeChanged=false );

protected:

  /// protected constructor to derive private implementations
  ctkDICOMDatasetViewerWidget( ctkDICOMDatasetViewerWidgetPrivate & pvt,
    QWidget* parent=0 );

private:

  QScopedPointer< ctkDICOMDatasetViewerWidgetPrivate > d_ptr;

  Q_DECLARE_PRIVATE( ctkDICOMDatasetViewerWidget );

  Q_DISABLE_COPY( ctkDICOMDatasetViewerWidget );

};

#endif
