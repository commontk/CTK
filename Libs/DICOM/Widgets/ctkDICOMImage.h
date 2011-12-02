/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef __ctkDICOMImage_h
#define __ctkDICOMImage_h

// Qt includes
#include <QObject>
#include <QImage>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMImagePrivate;
class DicomImage;

/// \ingroup DICOM_Widgets
///
/// \brief Wrapper around a DCMTK DicomImage.
///
/// This class wraps a DicomImage object and exposes it as a Qt class.
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMImage : public QObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned long frameCount READ frameCount);
public:
  ///  \brief Construct a ctkDICOMImage
  /// The dicomImage pointer must remain valid during all the life of
  /// the constructed ctkDICOMImage.
  ///
  explicit ctkDICOMImage(DicomImage* dicomImage, QObject* parent = 0);
  virtual ~ctkDICOMImage();

  ///
  /// \brief Returns the pointer on the dicom image given in the constructor.
  ///
  /// This is provided as a utility function. Do not delete the returned
  /// pointer.
  /// TBD: Return a "const DicomImage*" instead?
  ///
  DicomImage* dicomImage() const;

  ///
  /// \brief Returns a specific frame of the dicom image
  ///
  QImage frame(int frame = 0) const;

  ///
  /// \brief Returns the number of frames contained in the dicom image.
  /// \sa DicomImage::getFrameCount()
  ///
  /// Please note that this function does not return the number of frames
  /// stored in the DICOM file/dataset. It rather refers to the number of
  /// frames processed by this class.
  ///
  unsigned long frameCount() const;

protected:
  QScopedPointer<ctkDICOMImagePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMImage);
  Q_DISABLE_COPY(ctkDICOMImage);
};

#endif
