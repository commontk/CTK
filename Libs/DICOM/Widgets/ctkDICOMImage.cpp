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

// Qt includes
#include <QDebug>
#include <QString>

// ctkDICOMCore includes
#include "ctkDICOMImage.h"
#include "ctkLogger.h"

// DCMTK includes
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/ofstd/ofbmanip.h>

static ctkLogger logger ( "org.commontk.dicom.DICOMImage" );
struct Node;

//------------------------------------------------------------------------------
class ctkDICOMImagePrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMImage);
public:
  ctkDICOMImagePrivate(ctkDICOMImage&);

  ::DicomImage* DicomImage;

protected:
  ctkDICOMImage* const q_ptr;

private:
  Q_DISABLE_COPY(ctkDICOMImagePrivate);
};

//------------------------------------------------------------------------------
ctkDICOMImagePrivate::ctkDICOMImagePrivate(ctkDICOMImage& o):q_ptr(&o)
{
}

//------------------------------------------------------------------------------
ctkDICOMImage::ctkDICOMImage(DicomImage* dicomImage, QObject* parentValue)
  : d_ptr(new ctkDICOMImagePrivate(*this))
{
  Q_UNUSED(parentValue);
  Q_D(ctkDICOMImage);
  d->DicomImage = dicomImage;
  if (d->DicomImage)
    // Select first window defined in image. If none, compute min/max window as best guess.
    // Only relevant for monochrome.
    if (d->DicomImage->isMonochrome())
    {
        if (d->DicomImage->getWindowCount() > 0)
        {
          d->DicomImage->setWindow(0);
        }
        else
        {
          d->DicomImage->setMinMaxWindow(OFTrue /* ignore extreme values */);
        }
    }
}

//------------------------------------------------------------------------------
ctkDICOMImage::~ctkDICOMImage()
{
}

//------------------------------------------------------------------------------
unsigned long ctkDICOMImage::frameCount() const
{
  Q_D(const ctkDICOMImage);
  if (d->DicomImage)
    {
    return d->DicomImage->getFrameCount();
    }
  return 0;
}

//------------------------------------------------------------------------------
DicomImage* ctkDICOMImage::dicomImage() const
{
  Q_D(const ctkDICOMImage);
  return d->DicomImage;
}

//------------------------------------------------------------------------------
QImage ctkDICOMImage::frame(int frame) const
{
  Q_D(const ctkDICOMImage);

  // this way of converting the dicom image to a qpixmap was adopted from some code from
  // the DCMTK forum, posted by Joerg Riesmayer, see http://forum.dcmtk.org/viewtopic.php?t=120
  QImage image;
  if ((d->DicomImage != NULL) && (d->DicomImage->getStatus() == EIS_Normal))
    {
    /* get image extension */
    const unsigned long width = d->DicomImage->getWidth();
    const unsigned long height = d->DicomImage->getHeight();
    QString header = QString("P5 %1 %2 255\n").arg(width).arg(height);
    const unsigned long offset = header.length();
    const unsigned long length = width * height + offset;
    /* create output buffer for DicomImage class */
    QByteArray buffer;
    buffer.append(header);
    buffer.resize(length);

    /* copy PGM header to buffer */

    if (d->DicomImage->getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, frame))
      {

      if (!image.loadFromData( buffer ))
        {
        logger.error("QImage couldn't created");
        }
      }
    }
  return image;
}
