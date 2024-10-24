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

#ifndef __ctkDICOMThumbnailGenerator_h
#define __ctkDICOMThumbnailGenerator_h

// Qt includes
#include <QColor>
class QImage;

// ctkDICOMWidgets includes
#include "ctkDICOMAbstractThumbnailGenerator.h"
#include "ctkDICOMCoreExport.h"
class ctkDICOMThumbnailGeneratorPrivate;

// DCMTK includes
class DicomImage;

/// \ingroup DICOM_Core
///
/// \brief Thumbnail generator class
class CTK_DICOM_CORE_EXPORT ctkDICOMThumbnailGenerator : public ctkDICOMAbstractThumbnailGenerator
{
  Q_OBJECT
  Q_PROPERTY(int width READ width WRITE setWidth)
  Q_PROPERTY(int height READ height WRITE setHeight)
  Q_PROPERTY(bool smoothResize READ smoothResize WRITE setSmoothResize)

public:
  ///  \brief Construct a ctkDICOMThumbnailGenerator object
  explicit ctkDICOMThumbnailGenerator(QObject* parent = 0);
  virtual ~ctkDICOMThumbnailGenerator();

  virtual bool generateThumbnail(DicomImage* dcmImage, const QString& thumbnailPath,
                                 QColor backgroundColor = Qt::darkGray);

  Q_INVOKABLE bool generateThumbnail(DicomImage *dcmImage, QImage& image);
  Q_INVOKABLE bool generateThumbnail(const QString& dcmImagePath, QImage& image);
  Q_INVOKABLE bool generateThumbnail(const QString& dcmImagePath, const QString& thumbnailPath);

  /// Generate a blank thumbnail image (currently a solid gray box of the requested thumbnail size).
  /// It can be used as a placeholder for invalid images or duringan image is loaded.
  Q_INVOKABLE void generateBlankThumbnail(QImage& image, QColor backgroundColor = Qt::darkGray);
  Q_INVOKABLE virtual void generateDocumentThumbnail(const QString &thumbnailPath,
                                                     QColor backgroundColor = Qt::darkGray);

  /// Set thumbnail width
  void setWidth(int width);
  /// Get thumbnail width
  int width() const;
  /// Set thumbnail height
  void setHeight(int height);
  /// Get thumbnail height
  int height() const;
  /// Set thumbnail resize method
  /// \param on Smooth resize if true, fast if false. False by default
  void setSmoothResize(bool on);
  /// Get thumbnail height
  bool smoothResize() const;

protected:
  QScopedPointer<ctkDICOMThumbnailGeneratorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailGenerator);
  Q_DISABLE_COPY(ctkDICOMThumbnailGenerator);
};

#endif
