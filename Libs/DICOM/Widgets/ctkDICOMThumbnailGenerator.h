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

// CTK includes
#include "ctkDICOMWidgetsExport.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"

class ctkDICOMThumbnailGeneratorPrivate;
class DicomImage;

/// \ingroup DICOM_Widgets
///
/// \brief  thumbnail generator class
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMThumbnailGenerator : public ctkDICOMAbstractThumbnailGenerator
{
  Q_OBJECT
public:
  ///  \brief Construct a ctkDICOMThumbnailGenerator object
  ///
  explicit ctkDICOMThumbnailGenerator(QObject* parent = 0);
  virtual ~ctkDICOMThumbnailGenerator();

  virtual bool generateThumbnail(DicomImage* dcmImage, const QString& path );

protected:
  QScopedPointer<ctkDICOMThumbnailGeneratorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailGenerator);
  Q_DISABLE_COPY(ctkDICOMThumbnailGenerator);
};

#endif
