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

public:
  ctkDICOMThumbnailGeneratorPrivate(ctkDICOMThumbnailGenerator&);
  virtual ~ctkDICOMThumbnailGeneratorPrivate();

protected:
  ctkDICOMThumbnailGenerator* const q_ptr;

private:
  Q_DISABLE_COPY( ctkDICOMThumbnailGeneratorPrivate );
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
  Q_UNUSED(parentValue);
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGenerator::~ctkDICOMThumbnailGenerator()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(DicomImage *dcmImage, const QString &path){
    QImage image;
    // Check whether we have a valid image
    EI_Status result = dcmImage->getStatus();
    if (result != EIS_Normal)
    {
      logger.error(QString("Rendering of DICOM image failed for thumbnail failed: ") + DicomImage::getString(result));
      return false;
    }
    // Select first window defined in image. If none, compute min/max window as best guess.
    // Only relevant for monochrome.
    if (dcmImage->isMonochrome())
    {
        if (dcmImage->getWindowCount() > 0)
        {
          dcmImage->setWindow(0);
        }
        else
        {
          dcmImage->setMinMaxWindow(OFTrue /* ignore extreme values */);
        }
    }
    /* get image extension and prepare image header */
    const unsigned long width = dcmImage->getWidth();
    const unsigned long height = dcmImage->getHeight();
    unsigned long offset = 0;
    unsigned long length = 0;
    QString header;

    if (dcmImage->isMonochrome())
    {
      // write PGM header (binary monochrome image format)
      header = QString("P5 %1 %2 255\n").arg(width).arg(height);
      offset = header.length();
      length = width * height + offset;
    }
    else
    {
      // write PPM header (binary color image format)
      header = QString("P6 %1 %2 255\n").arg(width).arg(height);
      offset = header.length();
      length = width * height * 3 /* RGB */ + offset;
    }
    /* create output buffer for DicomImage class */
    QByteArray buffer;
    /* copy header to output buffer and resize it for pixel data */
    buffer.append(header);
    buffer.resize(length);

    /* render pixel data to buffer */
    if (dcmImage->getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0))
    {  
      if (!image.loadFromData( buffer ))
        {
            logger.error("QImage couldn't created");
            return false;
        }
    }
    image.scaled(128,128,Qt::KeepAspectRatio).save(path,"PNG");
    return true;
}
