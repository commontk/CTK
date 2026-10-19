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

// ctkDICOMCore includes
#include "ctkDICOMAbstractThumbnailGenerator.h"
#include "ctkLogger.h"

// Qt includes
#include <QDir>

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>

static ctkLogger logger ( "org.commontk.dicom.DICOMAbstractThumbnailGenerator" );
struct Node;

//------------------------------------------------------------------------------
class ctkDICOMAbstractThumbnailGeneratorPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMAbstractThumbnailGenerator);
protected:
  ctkDICOMAbstractThumbnailGenerator* const q_ptr;

public:
  ctkDICOMAbstractThumbnailGeneratorPrivate(ctkDICOMAbstractThumbnailGenerator&);
  virtual ~ctkDICOMAbstractThumbnailGeneratorPrivate();
};

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGeneratorPrivate::ctkDICOMAbstractThumbnailGeneratorPrivate(ctkDICOMAbstractThumbnailGenerator& o):q_ptr(&o)
{
    Q_UNUSED(o);
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGeneratorPrivate::~ctkDICOMAbstractThumbnailGeneratorPrivate()
{

}


//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator::ctkDICOMAbstractThumbnailGenerator(QObject* parentValue)
  : d_ptr(new ctkDICOMAbstractThumbnailGeneratorPrivate(*this))
{
    Q_UNUSED(parentValue);
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator::~ctkDICOMAbstractThumbnailGenerator()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMAbstractThumbnailGenerator::hasPixelData(const QString& dicomFilePath)
{
  // The values larger than 4 kB (the pixel data of an image) are left in the file
  DcmFileFormat fileFormat;
  if (fileFormat.loadFile(QDir::toNativeSeparators(dicomFilePath).toUtf8().data(),
                          EXS_Unknown, EGL_noChange, 4096).bad())
  {
    // Not readable: let the rendering report it
    return true;
  }
  DcmDataset* dataset = fileFormat.getDataset();
  return dataset->tagExists(DCM_PixelData) ||
         dataset->tagExists(DCM_FloatPixelData) ||
         dataset->tagExists(DCM_DoubleFloatPixelData);
}
