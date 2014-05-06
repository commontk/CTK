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

#ifndef __ctkDICOMItemView_h
#define __ctkDICOMItemView_h

/// Qt includes
#include <QWidget>
#include <QImage>
#include <QModelIndex>

/// CTK includes
#include "ctkQImageView.h"
#include "ctkPimpl.h"
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMItemViewPrivate;
class DicomImage;

/// \ingroup DICOM_Widgets
///
/// ctkDICOMItemView is the base class of image viewer widgets.
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMItemView
: public ctkQImageView
{

  Q_OBJECT

public:

  /// Superclass typedef
  typedef ctkQImageView Superclass;

  /// Constructor
  ctkDICOMItemView( QWidget * parent = 0 );
  
  /// Destructor
  virtual ~ctkDICOMItemView( void );

  void setDatabaseDirectory(const QString& directory);

  QModelIndex currentImageIndex();

Q_SIGNALS:

  void requestNextImage();
  void requestPreviousImage();

  void imageDisplayed(int imageID, int count);

public Q_SLOTS:

  void addImage( const QImage & image );

  void addImage( DicomImage & dcmImage, bool defaultIntensity = true);

  void onModelSelected(const QModelIndex& index);

  void displayImage(int imageIndex);

  virtual void update( bool zoomChanged=false, bool sizeChanged=false );

protected:

  /// protected constructor to derive private implementations
  ctkDICOMItemView( ctkDICOMItemViewPrivate & pvt,
    QWidget* parent=0 );

  /// re-implemented QWidget event functions
  virtual void mousePressEvent( QMouseEvent * event );
  virtual void	mouseMoveEvent( QMouseEvent * event );

private:

  QScopedPointer< ctkDICOMItemViewPrivate > d_ptr;

  Q_DECLARE_PRIVATE( ctkDICOMItemView );

  Q_DISABLE_COPY( ctkDICOMItemView );

  void emitImageDisplayedSignal(int imageID, int count);

};

#endif
