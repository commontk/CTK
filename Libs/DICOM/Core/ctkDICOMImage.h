/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef __ctkDICOMImage_h
#define __ctkDICOMImage_h

// Qt includes 
#include <QObject>
#include <QPixmap>

#include "ctkDICOMCoreExport.h"

class ctkDICOMImagePrivate;
class DicomImage;

class CTK_DICOM_CORE_EXPORT ctkDICOMImage : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMImage(DicomImage* dicomImage, QObject* parent = 0);
  virtual ~ctkDICOMImage();
  DicomImage* getDicomImage() const;
  QImage getImage(int frame = 0) const;
  unsigned long frameCount() const;
  Q_PROPERTY(unsigned long frameCount READ frameCount);

protected:
  QScopedPointer<ctkDICOMImagePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMImage);
  Q_DISABLE_COPY(ctkDICOMImage);
};

#endif
