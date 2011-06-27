/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#include <iostream>

// DCMTK includes
#include <dcmimage.h>

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

protected:

  ctkDICOMDatasetView* const q_ptr;

public:

  ctkDICOMDatasetViewPrivate( ctkDICOMDatasetView& object );

  QString databaseDirectory;

  QModelIndex currentImageIndex;

  QPoint oldMousePos;

  double dicomIntensityLevel;

  double dicomIntensityWindow;

  void init();

  void setImage(const QModelIndex& imageIndex, bool defaultIntensity = true);

  void onPatientModelSelected(const QModelIndex& index);
  void onStudyModelSelected(const QModelIndex& index);
  void onSeriesModelSelected(const QModelIndex& index);
  void onImageModelSelected(const QModelIndex& index);
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

  this->dicomIntensityLevel = 0;

  this->dicomIntensityWindow = 0;

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

        QString dicomPath = this->databaseDirectory;
        dicomPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        dicomPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        dicomPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(dicomPath).exists()){
            DicomImage dcmImage( dicomPath.toStdString().c_str() );

            q->clearImages();
            q->addImage(dcmImage, defaultIntensity);
            this->currentImageIndex = imageIndex;
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

        if(index.parent() == this->currentImageIndex.parent()){
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

    d->databaseDirectory = directory;
}

// -------------------------------------------------------------------------
QModelIndex ctkDICOMDatasetView::currentImageIndex(){
    Q_D(ctkDICOMDatasetView);

    return d->currentImageIndex;
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
    if ((dcmImage.getStatus() == EIS_Normal)){
        if(defaultIntensity){
            dcmImage.setWindow(0);
            dcmImage.getWindow(d->dicomIntensityLevel, d->dicomIntensityWindow);
        }else{
            dcmImage.setWindow(d->dicomIntensityLevel, d->dicomIntensityWindow);
        }
        /* get image extension */
        const unsigned long width = dcmImage.getWidth();
        const unsigned long height = dcmImage.getHeight();
        QString header = QString("P5 %1 %2 255\n").arg(width).arg(height);
        const unsigned long offset = header.length();
        const unsigned long length = width * height + offset;
        /* create output buffer for DicomImage class */
        QByteArray buffer;
        buffer.append(header);
        buffer.resize(length);

        /* copy PGM header to buffer */

        if (dcmImage.getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0)){
            if (!image.loadFromData( buffer )){
                logger.error("QImage couldn't created");
            }
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
    d->oldMousePos = event->pos();
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::mouseMoveEvent(QMouseEvent* event){
    Q_D(ctkDICOMDatasetView);

    if(event->buttons() == Qt::RightButton){
        event->accept();
        QPoint nowPos = event->pos();
        if(nowPos.y() > d->oldMousePos.y()){
            emit requestNextImage();
            d->oldMousePos = event->pos();
        }else if(nowPos.y() < d->oldMousePos.y()){
            emit requestPreviousImage();
            d->oldMousePos = event->pos();
        }
    }else if(event->buttons() == Qt::MidButton){
        event->accept();
        QPoint nowPos = event->pos();

        this->setZoom(this->zoom() - (nowPos.y()-d->oldMousePos.y())/100.0);

        d->oldMousePos = event->pos();
    }else if(event->buttons() == Qt::LeftButton){
        event->accept();
        QPoint nowPos = event->pos();

        d->dicomIntensityWindow += (5*(nowPos.x()-d->oldMousePos.x()));
        d->dicomIntensityLevel -= (5*(nowPos.y()-d->oldMousePos.y()));

        d->setImage(d->currentImageIndex, false);

        d->oldMousePos = event->pos();
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
