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

// Qt include
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"
#include "ui_ctkThumbnailListWidget.h"

//ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMFilterProxyModel.h"
#include "ctkDICOMModel.h"

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailListWidget.h"
#include "ctkThumbnailWidget.h"

// STD includes
#include <iostream>

// DCMTK includes
#include <dcmimage.h>

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMThumbnailListWidget");

//----------------------------------------------------------------------------
class ctkDICOMThumbnailListWidgetPrivate
{
public:
  ctkDICOMThumbnailListWidgetPrivate(ctkDICOMThumbnailListWidget* parent);

  QString DatabaseDirectory;
  QModelIndex CurrentSelectedModel;

  void addThumbnailWidget(const QModelIndex &imageIndex, const QModelIndex& sourceIndex, const QString& text);

  void onPatientModelSelected(const QModelIndex &index);
  void onStudyModelSelected(const QModelIndex &index);
  void onSeriesModelSelected(const QModelIndex &index);

  ctkDICOMThumbnailListWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailListWidget);
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidgetPrivate::ctkDICOMThumbnailListWidgetPrivate(ctkDICOMThumbnailListWidget* parent): q_ptr(parent){

}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidgetPrivate::onPatientModelSelected(const QModelIndex &index){
    QModelIndex patientIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
    {
        model->fetchMore(patientIndex);
        int studyCount = model->rowCount(patientIndex);

        for(int i=0; i<studyCount; i++)
        {
            QModelIndex studyIndex = patientIndex.child(i, 0);
            QModelIndex seriesIndex = studyIndex.child(0, 0);
            model->fetchMore(seriesIndex);
            int imageCount = model->rowCount(seriesIndex);
            QModelIndex imageIndex = seriesIndex.child(imageCount/2, 0);

            QString thumbnailPath = this->DatabaseDirectory +
                                    "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

            if(QFile(thumbnailPath).exists())
            {
                this->addThumbnailWidget(imageIndex, studyIndex, model->data(studyIndex, Qt::DisplayRole).toString());
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::onStudyModelSelected(const QModelIndex &index){
    QModelIndex studyIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
    {
        model->fetchMore(studyIndex);
        int seriesCount = model->rowCount(studyIndex);

        for(int i=0; i<seriesCount; i++)
        {
            QModelIndex seriesIndex = studyIndex.child(i, 0);
            model->fetchMore(seriesIndex);
            int imageCount = model->rowCount(seriesIndex);
            QModelIndex imageIndex = seriesIndex.child(imageCount/2, 0);

            QString thumbnailPath = this->DatabaseDirectory +
                                    "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

            if (QFile(thumbnailPath).exists())
            {
                this->addThumbnailWidget(imageIndex, seriesIndex, model->data(seriesIndex, Qt::DisplayRole).toString());
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::onSeriesModelSelected(const QModelIndex &index){
    QModelIndex studyIndex = index.parent();
    QModelIndex seriesIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
    {
        model->fetchMore(seriesIndex);

        int imageCount = model->rowCount(seriesIndex);
        logger.debug(QString("Thumbs: %1").arg(imageCount));
        for (int i = 0 ; i < imageCount ; i++ )
        {
            QModelIndex imageIndex = seriesIndex.child(i,0);

            QString thumbnailPath = this->DatabaseDirectory +
                                    "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

            if(QFile(thumbnailPath).exists())
            {
                this->addThumbnailWidget(imageIndex, imageIndex, QString("Image %1").arg(i));
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::addThumbnailWidget(const QModelIndex& imageIndex, const QModelIndex& sourceIndex, const QString &text){
    Q_Q(ctkDICOMThumbnailListWidget);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(imageIndex.model()));

    if(model)
    {
        QModelIndex seriesIndex = imageIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();

        QString thumbnailPath = this->DatabaseDirectory +
                                "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

        ctkThumbnailWidget* widget = new ctkThumbnailWidget(q->ui->ScrollAreaContentWidget);
        QString widgetLabel = text;
        widget->setText( widgetLabel );
        QPixmap pix(thumbnailPath);
        logger.debug("Setting pixmap to " + thumbnailPath);
        if(q->ThumbnailWidth > 0){
          widget->setMaximumWidth(q->ThumbnailWidth);
          widget->setMinimumWidth(q->ThumbnailWidth);
        }
        widget->setPixmap(pix);
        widget->setSourceIndex(sourceIndex);
        q->ui->ScrollAreaContentWidget->layout()->addWidget(widget);

        q->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkThumbnailWidget)));
        q->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), q, SIGNAL(selected(ctkThumbnailWidget)));
        q->connect(widget, SIGNAL(doubleClicked(ctkThumbnailWidget)), q, SIGNAL(doubleClicked(ctkThumbnailWidget)));
    }
}

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::ctkDICOMThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
									   d_ptr(new ctkDICOMThumbnailListWidgetPrivate(this))
{
    Q_D(ctkDICOMThumbnailListWidget);

    ui->ScrollAreaContentWidget->setLayout(new ctkFlowLayout);
    qobject_cast<ctkFlowLayout*>(ui->ScrollAreaContentWidget->layout())->setHorizontalSpacing(4);
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::~ctkDICOMThumbnailListWidget()
{

}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::setDatabaseDirectory(const QString &directory){
    Q_D(ctkDICOMThumbnailListWidget);

    d->DatabaseDirectory = directory;
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::selectThumbnailFromIndex(const QModelIndex &index){
    Q_D(ctkDICOMThumbnailListWidget);

    if(!d->CurrentSelectedModel.isValid())
      {
      return;
      }
    if(index.parent() != d->CurrentSelectedModel)
      {
      return;
      }

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
    {
        int count = ui->ScrollAreaContentWidget->layout()->count();

        for(int i=0; i<count; i++)
        {
            ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(ui->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
            if(thumbnailWidget->sourceIndex() == index){
                thumbnailWidget->setSelected(true);
                ui->ScrollArea->ensureWidgetVisible(thumbnailWidget);
            }
            else
            {
                thumbnailWidget->setSelected(false);
            }
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::onModelSelected(const QModelIndex &index){
    Q_D(ctkDICOMThumbnailListWidget);

    this->reset();

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
    {
        QModelIndex index0 = index.sibling(index.row(), 0);

        d->CurrentSelectedModel = index0;

        if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) )
        {
            d->onPatientModelSelected(index0);
        }
        else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) )
        {
            d->onStudyModelSelected(index0);
        }
        else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) )
        {
            d->onSeriesModelSelected(index0);
        }
    }

    this->selectThumbnail(0);
}
