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

// Qt includes
#include <QDebug>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QtSvg/QSvgRenderer>

// DCMTK includes
#include "dcmtk/dcmimgle/dcmimage.h"

//------------------------------------------------------------------------------
// Using dcmtk root log4cplus logger instead of ctkLogger because with ctkDICOMJobsAppender (dcmtk::log4cplus::Appender),
// logging is filtered by threadID and reported in the GUI per job.
dcmtk::log4cplus::Logger rootLogThumbnailGenerator = dcmtk::log4cplus::Logger::getRoot();

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
  emit this->widthChanged(width);
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
  emit this->heightChanged(height);
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
  emit this->smoothResizeChanged(on);
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(DicomImage *dcmImage, QImage& image)
{
  Q_D(ctkDICOMThumbnailGenerator);

  // Check whether we have a valid image
  EI_Status result = dcmImage->getStatus();
  if (result != EIS_Normal)
  {
    QString warn = QString("Rendering of DICOM image failed for thumbnail failed: %1").arg(DicomImage::getString(result));
    DCMTK_LOG4CPLUS_WARN_STR(rootLogThumbnailGenerator, warn.toStdString().c_str());
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
  buffer.append(header.toUtf8().data());
  buffer.resize(length);

  /* render pixel data to buffer */
  if (dcmImage->getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0))
  {
    if (!image.loadFromData( buffer ))
    {
      DCMTK_LOG4CPLUS_ERROR_STR(rootLogThumbnailGenerator, "QImage couldn't created");
      return false;
    }
  }
  image = image.scaled( d->Width, d->Height, Qt::KeepAspectRatio,
    (d->SmoothResize ? Qt::SmoothTransformation : Qt::FastTransformation) );
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(DicomImage *dcmImage, const QString &thumbnailPath,
                                                   QColor backgroundColor)
{
  QImage image;
  if (this->generateThumbnail(dcmImage, image))
  {
    return image.save(thumbnailPath, "PNG");
  }

  DCMTK_LOG4CPLUS_DEBUG_STR(rootLogThumbnailGenerator, "Thumbnail generation failed, using a document icon instead.");
  this->generateDocumentThumbnail(thumbnailPath, backgroundColor);
  return false;
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(const QString& dcmImagePath, QImage& image)
{
  DicomImage dcmImage(QDir::toNativeSeparators(dcmImagePath).toUtf8().data());
  return this->generateThumbnail(&dcmImage, image);
}

//------------------------------------------------------------------------------
bool ctkDICOMThumbnailGenerator::generateThumbnail(const QString& dcmImagePath, const QString& thumbnailPath)
{
  DicomImage dcmImage(QDir::toNativeSeparators(dcmImagePath).toUtf8().data());
  return this->generateThumbnail(&dcmImage, thumbnailPath);
}

//------------------------------------------------------------------------------
void ctkDICOMThumbnailGenerator::generateBlankThumbnail(QImage& image, QColor backgroundColor)
{
  Q_D(ctkDICOMThumbnailGenerator);
  if (image.width() != d->Width || image.height() != d->Height)
  {
    image = QImage(d->Width, d->Height, QImage::Format_RGB32);
  }
  image.fill(backgroundColor);
}

//------------------------------------------------------------------------------
void ctkDICOMThumbnailGenerator::generateDocumentThumbnail(const QString &thumbnailPath,
                                                           QColor backgroundColor)
{
  QImage image;
  this->generateBlankThumbnail(image, backgroundColor);
  QPixmap pixmap = QPixmap::fromImage(image);
  QPainter painter;
  if (painter.begin(&pixmap))
  {
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer renderer(QString(":Icons/text_document.svg"));
    renderer.render(&painter);
    painter.end();
  }
  image = pixmap.toImage();
  image.save(thumbnailPath, "PNG");
}
