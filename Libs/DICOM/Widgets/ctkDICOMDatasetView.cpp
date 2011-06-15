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
#include "ctkDICOMImage.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMDatasetView.h"

// Qt includes
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>

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

  void init();

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

  /*
  this->Window->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Window);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);
  */
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetViewPrivate::onPatientModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMDatasetView);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex patientIndex = index;
        QModelIndex studyIndex = patientIndex.child(0,0);
        QModelIndex seriesIndex = studyIndex.child(0,0);
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        QString thumbnailPath = this->databaseDirectory;
        thumbnailPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(thumbnailPath).exists())
        {
          DicomImage dcmImage( thumbnailPath.toStdString().c_str() );
          ctkDICOMImage ctkImage( & dcmImage );
          q->clearImages();
          q->addImage( ctkImage );
          this->currentImageIndex = imageIndex;
        }else{
          q->clearImages();
        }
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
        QModelIndex seriesIndex = studyIndex.child(0,0);
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        QString thumbnailPath = this->databaseDirectory;
        thumbnailPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(thumbnailPath).exists())
        {
          DicomImage dcmImage( thumbnailPath.toStdString().c_str() );
          ctkDICOMImage ctkImage( & dcmImage );
          q->clearImages();
          q->addImage( ctkImage );
          this->currentImageIndex = imageIndex;
        }else{
          q->clearImages();
        }
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
        QModelIndex studyIndex = seriesIndex.parent();
        model->fetchMore(seriesIndex);
        int imageCount = model->rowCount(seriesIndex);
        QModelIndex imageIndex = seriesIndex.child(imageCount/2,0);

        QString thumbnailPath = this->databaseDirectory;
        thumbnailPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(thumbnailPath).exists())
        {
          DicomImage dcmImage( thumbnailPath.toStdString().c_str() );
          ctkDICOMImage ctkImage( & dcmImage );
          q->clearImages();
          q->addImage( ctkImage );
          this->currentImageIndex = imageIndex;
        }else{
          q->clearImages();
        }
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
        QModelIndex seriesIndex = imageIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();

        QString thumbnailPath = this->databaseDirectory;
        thumbnailPath.append("/dicom/").append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString());
        thumbnailPath.append("/").append(model->data(imageIndex ,ctkDICOMModel::UIDRole).toString());

        if (QFile(thumbnailPath).exists())
        {
          DicomImage dcmImage( thumbnailPath.toStdString().c_str() );
          ctkDICOMImage ctkImage( & dcmImage );
          q->clearImages();
          q->addImage( ctkImage );
          this->currentImageIndex = imageIndex;
        }else{
          q->clearImages();
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
void ctkDICOMDatasetView::addImage( const ctkDICOMImage & image )
{
  for( unsigned int i=0; i<image.frameCount(); ++i )
    {
    Superclass::addImage( image.frame( i ) );
    }
}

// -------------------------------------------------------------------------
void ctkDICOMDatasetView::addImage( const QImage & image )
{
  Superclass::addImage( image );
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

        double iW = this->intensityWindow();
        double iL = this->intensityLevel();

        this->setIntensityWindowLevel(iW + (nowPos.x()-d->oldMousePos.x()),
                                      iL - (nowPos.y()-d->oldMousePos.y()));

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


        if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::PatientType ){
            d->onPatientModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::StudyType ){
            d->onStudyModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::SeriesType ){
            d->onSeriesModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::ImageType ){
            d->onImageModelSelected(index0);
        }
    }
}
