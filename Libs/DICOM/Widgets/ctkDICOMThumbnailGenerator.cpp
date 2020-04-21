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
#include <QDebug>
#include <QDir>
#include <QImage>

// DCMTK includes
#include "dcmtk/dcmimgle/dcmimage.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMThumbnailGenerator" );

//------------------------------------------------------------------------------
class ctkDICOMThumbnailGeneratorPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailGenerator);

public:
  ctkDICOMThumbnailGeneratorPrivate(ctkDICOMThumbnailGenerator&);
  virtual ~ctkDICOMThumbnailGeneratorPrivate();

protected:
  ctkDICOMThumbnailGenerator* const q_ptr;

  int Width;
  int Height;
  bool SmoothResize;

private:
  Q_DISABLE_COPY( ctkDICOMThumbnailGeneratorPrivate );
};

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorPrivate::ctkDICOMThumbnailGeneratorPrivate(ctkDICOMThumbnailGenerator& o)
  : q_ptr(&o)
  , Width(256)
  , Height(256)
  , SmoothResize(false)
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
int ctkDICOMThumbnailGenerator::width()const
{
  Q_D(const ctkDICOMThumbnailGenerator);
  return d->Width;
}

//------------------------------------------------------------------------------
void ctkDICOMThumbnailGenerator::setWidth(int width)
{
  Q_D(ctkDICOMThumbnailGenerator);
  d->Width = width;
}

//------------------------------------------------------------------------------
int ctkDICOMThumbnailGenerator::height()const
{
  Q_D(const ctkDICOMThumbnailGenerator);
  return d->Height;
}

//------------------------------------------------------------------------------
void ctkDICOMThumbnailGenerator::setHeight(int height)
{
  Q_D(ctkDICOMThumbnailGenerator);
  d->Height = height;
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::smoothResize()const
{
  Q_D(const ctkDICOMThumbnailGenerator);
  return d->SmoothResize;
}

//------------------------------------------------------------------------------
void ctkDICOMThumbnailGenerator::setSmoothResize(bool on)
{
  Q_D(ctkDICOMThumbnailGenerator);
  d->SmoothResize = on;
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(DicomImage *dcmImage, const QString &path)
{
  Q_D(ctkDICOMThumbnailGenerator);

  QImage image;
  // Check whether we have a valid image
  EI_Status result = dcmImage->getStatus();
  if (result != EIS_Normal)
  {
    qCritical() << Q_FUNC_INFO << QString("Rendering of DICOM image failed for thumbnail failed: ") + DicomImage::getString(result);
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
      qCritical() << Q_FUNC_INFO << "QImage couldn't created";
      return false;
    }
  }
  image.scaled( d->Width, d->Height, Qt::KeepAspectRatio,
    (d->SmoothResize ? Qt::SmoothTransformation : Qt::FastTransformation) ).save(path,"PNG");
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(const QString dcmImagePath, const QString& thumbnailPath)
{
  DicomImage dcmImage(QDir::toNativeSeparators(dcmImagePath).toUtf8());
  return this->generateThumbnail(&dcmImage, thumbnailPath); 
}
