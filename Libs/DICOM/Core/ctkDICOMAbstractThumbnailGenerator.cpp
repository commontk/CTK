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
