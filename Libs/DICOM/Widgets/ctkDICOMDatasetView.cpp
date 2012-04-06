/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#include <iostream>

// DCMTK includes
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmimage/diregist.h> /* Include color image support */

// CTK includes
#include "ctkLogger.h"
#include "ctkQImageView.h"

// ctkDICOMCore includes
#include "ctkDICOMFilterProxyModel.h"
#include "ctkDICOMModel.h"

// ctkDICOMWidgets includex
#include "ctkDICOMDatasetView.h"

// Qt includes
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMDatasetView");

//--------------------------------------------------------------------------
class ctkDICOMDatasetViewPrivate 
{

  Q_DECLARE_PUBLIC( ctkDICOMDatasetView );

public:

  ctkDICOMDatasetViewPrivate( ctkDICOMDatasetView& object );

  QString DatabaseDirectory;
  QModelIndex CurrentImageIndex;
  QPoint OldMousePos;
  double DicomIntensityLevel;
  double DicomIntensityWindow;
  bool AutoWindowLevel;

  void init();

  void setImage(const QModelIndex& imageIndex, bool defaultIntensity = true);

  void onPatientModelSelected(const QModelIndex& index);
  void onStudyModelSelected(const QModelIndex& index);
  void onSeriesModelSelected(const QModelIndex& index);
  void onImageModelSelected(const QModelIndex& index);

protected:
  ctkDICOMDatasetView* const q_ptr;

private:
  Q_DISABLE_COPY( ctkDICOMDatasetViewPrivate );
};

//--------------------------------------------------------------------------
ctkDICOMDatasetViewPrivate::ctkDICOMDatasetViewPrivate(
  ctkDICOMDatasetView& object )
  : q_ptr( & object )
{
}

//--------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::init()
{
  Q_Q( ctkDICOMDatasetView );

  q->setMouseTracking(true);

  this->DicomIntensityLevel = 0;

  this->DicomIntensityWindow = 0;

  this->AutoWindowLevel = true;

  /*
  this->Window->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Window);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);
  */
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::setImage(const QModelIndex &imageIndex, bool defaultIntensity){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(imageIndex.model()));

    if(model){
        QModelIndex seriesIndex = imageIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();

        QString dicomPath = this->DatabaseDirectory;
        dicomPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        dicomPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        dicomPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(dicomPath).exists()){
          DicomImage dcmImage(  QDir::toNativeSeparators(dicomPath).toStdString().c_str() );

            q->clearImages();
            q->addImage(dcmImage, defaultIntensity);
            this->CurrentImageIndex = imageIndex;

            q->emitImageDisplayedSignal(imageIndex.row(), model->rowCount(seriesIndex));
        }else{
            q->clearImages();
        }
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::onPatientModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex patientIndex = index;
        model->fetchMore(patientIndex);
        QModelIndex studyIndex = patientIndex.child(0,0);
        model->fetchMore(studyIndex);
        QModelIndex seriesIndex = studyIndex.child(0,0);
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        this->setImage(imageIndex);
    }else{
        q->clearImages();
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::onStudyModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex studyIndex = index;
        model->fetchMore(studyIndex);
        QModelIndex seriesIndex = studyIndex.child(0,0);
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        this->setImage(imageIndex);
    }else{
        q->clearImages();
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::onSeriesModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex seriesIndex = index;
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        this->setImage(imageIndex);
    }else{
        q->clearImages();
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::onImageModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex imageIndex = index;

        if(index.parent() == this->CurrentImageIndex.parent()){
            this->setImage(imageIndex, false);
        }else{
            this->setImage(imageIndex, true);
        }
    }else{
        q->clearImages();
    }
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::ctkDICOMDatasetView( QWidget* _parent )
  : Superclass( _parent ),
    d_ptr( new ctkDICOMDatasetViewPrivate( *this ) )
{
  Q_D( ctkDICOMDatasetView );
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::ctkDICOMDatasetView(
  ctkDICOMDatasetViewPrivate& pvt,
  QWidget* _parent)
  : Superclass(_parent), d_ptr(&pvt)
{
  Q_D(ctkDICOMDatasetView);
  d->init();
}

// -------------------------------------------------------------------------
ctkDICOMDatasetView::~ctkDICOMDatasetView()
{
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::setDatabaseDirectory(const QString &directory){
    Q_D(ctkDICOMDatasetView);

    d->DatabaseDirectory = directory;
}

// -------------------------------------------------------------------------
QModelIndex ctkDICOMDatasetView::currentImageIndex(){
    Q_D(ctkDICOMDatasetView);

    return d->CurrentImageIndex;
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::addImage( const QImage & image )
{
  Superclass::addImage( image );
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::addImage( DicomImage & dcmImage, bool defaultIntensity )
{
    Q_D(ctkDICOMDatasetView);
    QImage image;
    // Check whether we have a valid image
    EI_Status result = dcmImage.getStatus();
    if (result != EIS_Normal)
    {
      logger.error(QString("Rendering of DICOM image failed for thumbnail failed: ") + DicomImage::getString(result));
      return;
    }
    // Select first window defined in image. If none, compute min/max window as best guess.
    // Only relevant for monochrome
    if (d->AutoWindowLevel)
    {
      if (dcmImage.isMonochrome())
      {
          if (defaultIntensity && dcmImage.getWindowCount() > 0)
          {
            dcmImage.setWindow(0);
          }
          else
          {
            dcmImage.setMinMaxWindow(OFTrue /* ignore extreme values */);
            dcmImage.getWindow(d->DicomIntensityLevel, d->DicomIntensityWindow);
          }
      }
    } 
    else 
    {
      dcmImage.setWindow(d->DicomIntensityLevel, d->DicomIntensityWindow);
    }
    /* get image extension and prepare image header */
    const unsigned long width = dcmImage.getWidth();
    const unsigned long height = dcmImage.getHeight();
    unsigned long offset = 0;
    unsigned long length = 0;
    QString header;

    if (dcmImage.isMonochrome())
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
    if (dcmImage.getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0))
    {  
      if (!image.loadFromData( buffer ))
        {
            logger.error("QImage couldn't created");
        }
    }
    this->addImage(image);
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::update( bool zoomChanged,
  bool sizeChanged )
{
  Superclass::update( zoomChanged, sizeChanged );
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::mousePressEvent(QMouseEvent* event){
    Q_D(ctkDICOMDatasetView);

    event->accept();
    d->OldMousePos = event->pos();
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::mouseMoveEvent(QMouseEvent* event){
    Q_D(ctkDICOMDatasetView);

    if(event->buttons() == Qt::RightButton){
        event->accept();
        QPoint nowPos = event->pos();
        if(nowPos.y() > d->OldMousePos.y()){
            emit requestNextImage();
            d->OldMousePos = event->pos();
        }else if(nowPos.y() < d->OldMousePos.y()){
            emit requestPreviousImage();
            d->OldMousePos = event->pos();
        }
    }else if(event->buttons() == Qt::MidButton){
        event->accept();
        QPoint nowPos = event->pos();

        this->setZoom(this->zoom() - (nowPos.y()-d->OldMousePos.y())/100.0);

        d->OldMousePos = event->pos();
    }else if(event->buttons() == Qt::LeftButton){
        event->accept();
        QPoint nowPos = event->pos();

        d->DicomIntensityWindow += (5*(nowPos.x()-d->OldMousePos.x()));
        d->DicomIntensityLevel -= (5*(nowPos.y()-d->OldMousePos.y()));
        d->AutoWindowLevel = false;

        d->setImage(d->CurrentImageIndex, false);

        d->OldMousePos = event->pos();
    }

    Superclass::mouseMoveEvent(event);
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::onModelSelected(const QModelIndex &index){
    Q_D(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex index0 = index.sibling(index.row(), 0);

        if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) ){
            d->onPatientModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) ){
            d->onStudyModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) ){
            d->onSeriesModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::ImageType) ){
            d->onImageModelSelected(index0);
        }
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::displayImage(int imageIndex){
  Q_D(ctkDICOMDatasetView);

  if(d->CurrentImageIndex.isValid())
    {
      QModelIndex seriesIndex = d->CurrentImageIndex.parent();
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(seriesIndex.model()));

      if(model)
        {
          if(imageIndex >= 0 && imageIndex < model->rowCount(seriesIndex))
            {
            this->onModelSelected(model->index(imageIndex, 0, seriesIndex));
            }
          else
            {
            logger.debug("out of index");
            }
        }
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::emitImageDisplayedSignal(int imageID, int count){
  emit imageDisplayed(imageID, count);
}

