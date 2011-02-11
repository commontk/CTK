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

#ifndef __ctkDICOMDatasetView_h
#define __ctkDICOMDatasetView_h

/// Qt includes
#include <QWidget>
#include <QImage>

/// CTK includes
#include "ctkQImageView.h"
#include "ctkPimpl.h"
#include "ctkDICOMImage.h"
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMDatasetViewPrivate;

///
/// ctkDICOMDatasetView is the base class of image viewer widgets.
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMDatasetView
: public ctkQImageView
{

  Q_OBJECT

public:

  /// Superclass typedef
  typedef ctkQImageView Superclass;

  /// Constructor
  ctkDICOMDatasetView( QWidget * parent = 0 );
  
  /// Destructor
  virtual ~ctkDICOMDatasetView( void );

public slots:

  void addImage( const ctkDICOMImage & image );
  void addImage( const QImage & image );

  virtual void update( bool zoomChanged=false, bool sizeChanged=false );

protected:

  /// protected constructor to derive private implementations
  ctkDICOMDatasetView( ctkDICOMDatasetViewPrivate & pvt,
    QWidget* parent=0 );

private:

  QScopedPointer< ctkDICOMDatasetViewPrivate > d_ptr;

  Q_DECLARE_PRIVATE( ctkDICOMDatasetView );

  Q_DISABLE_COPY( ctkDICOMDatasetView );

};

#endif
