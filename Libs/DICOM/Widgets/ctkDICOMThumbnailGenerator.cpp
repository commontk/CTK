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

// ctkDICOMCore includes
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkLogger.h"

// Qt includes
#include <QImage>

// DCMTK includes
#include "dcmimage.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMThumbnailGenerator" );
struct Node;

//------------------------------------------------------------------------------
class ctkDICOMThumbnailGeneratorPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailGenerator);
protected:
  ctkDICOMThumbnailGenerator* const q_ptr;

public:
  ctkDICOMThumbnailGeneratorPrivate(ctkDICOMThumbnailGenerator&);
  virtual ~ctkDICOMThumbnailGeneratorPrivate();
};

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorPrivate::ctkDICOMThumbnailGeneratorPrivate(ctkDICOMThumbnailGenerator& o):q_ptr(&o)
{

}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorPrivate::~ctkDICOMThumbnailGeneratorPrivate()
{

}


//------------------------------------------------------------------------------
ctkDICOMThumbnailGenerator::ctkDICOMThumbnailGenerator(QObject* parentValue)
  : d_ptr(new ctkDICOMThumbnailGeneratorPrivate(*this))
{

}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGenerator::~ctkDICOMThumbnailGenerator()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(DicomImage *dcmImage, const QString &path){
    QImage image;
    if ((dcmImage->getStatus() == EIS_Normal))
    {
        dcmImage->setWindow(0);
        /* get image extension */
        const unsigned long width = dcmImage->getWidth();
        const unsigned long height = dcmImage->getHeight();
        QString header = QString("P5 %1 %2 255\n").arg(width).arg(height);
        const unsigned long offset = header.length();
        const unsigned long length = width * height + offset;
        /* create output buffer for DicomImage class */
        QByteArray buffer;
        buffer.append(header);
        buffer.resize(length);

        /* copy PGM header to buffer */

        if (dcmImage->getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0))
        {
            if (!image.loadFromData( buffer ))
            {
                logger.error("QImage couldn't created");
                return false;
            }
        }
    }
    image.scaled(128,128,Qt::KeepAspectRatio).save(path,"PNG");
    return true;
}
