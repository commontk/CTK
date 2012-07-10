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

#ifndef __ctkDICOMAbstractThumbnailGenerator_h
#define __ctkDICOMAbstractThumbnailGenerator_h

// Qt includes
#include <QObject>

#include "ctkDICOMCoreExport.h"

class ctkDICOMAbstractThumbnailGeneratorPrivate;
class DicomImage;

/// \ingroup DICOM_Core
///
/// \brief Abstract thumbnail generator class
///
class CTK_DICOM_CORE_EXPORT ctkDICOMAbstractThumbnailGenerator : public QObject
{
  Q_OBJECT
public:
  ///  \brief Construct a ctkDICOMAbstractThumbnailGenerator object
  ///
  explicit ctkDICOMAbstractThumbnailGenerator(QObject* parent = 0);
  virtual ~ctkDICOMAbstractThumbnailGenerator();

  virtual bool generateThumbnail(DicomImage* dcmImage, const QString& path ) = 0;

protected:
  QScopedPointer<ctkDICOMAbstractThumbnailGeneratorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMAbstractThumbnailGenerator);
  Q_DISABLE_COPY(ctkDICOMAbstractThumbnailGenerator);
};

#endif
