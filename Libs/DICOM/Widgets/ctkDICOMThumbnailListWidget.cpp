
// Qt include
#include <QGridLayout>
#include <QResizeEvent>
#include <QPushButton>
#include <QPixmap>
#include <QFile>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"

//ctkDICOMCore includes
#include "ctkDICOMModel.h"
#include "ctkDICOMDatabase.h"

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailListWidget.h"
#include "ui_ctkDICOMThumbnailListWidget.h"
#include "ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMThumbnailListWidget");

//----------------------------------------------------------------------------
class ctkDICOMThumbnailListWidgetPrivate: public Ui_ctkDICOMThumbnailListWidget
{
public:
  ctkDICOMThumbnailListWidgetPrivate(ctkDICOMThumbnailListWidget* parent): q_ptr(parent){
    
  }

  QString databaseDirectory;

  void clearAllThumbnails();

  void onPatientModelSelected(const QModelIndex &index);
  void onStudyModelSelected(const QModelIndex &index);
  void onSeriesModelSelected(const QModelIndex &index);

  ctkDICOMThumbnailListWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMThumbnailListWidget);
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidgetPrivate methods

void ctkDICOMThumbnailListWidgetPrivate::onPatientModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMThumbnailListWidget);

    QModelIndex patientIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        model->fetchMore(patientIndex);
        int studyCount = model->rowCount(patientIndex);

        for(int i=0; i<studyCount; i++){
            QModelIndex studyIndex = patientIndex.child(i, 0);
            QModelIndex seriesIndex = studyIndex.child(0, 0);
            QModelIndex imageIndex = seriesIndex.child(0, 0);

            QString thumbnail = this->databaseDirectory +
                                    "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

            if (QFile(thumbnail).exists()){
                ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(this->scrollAreaContentWidget);
                widget->setText( model->data(studyIndex, Qt::DisplayRole).toString() );
                QPixmap pix(thumbnail);
                logger.debug("Setting pixmap to " + thumbnail);
                widget->setPixmap(pix);
                widget->setSourceIndex(studyIndex);
                this->scrollAreaContentWidget->layout()->addWidget(widget);
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkDICOMThumbnailWidget)));
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SIGNAL(selected(ctkDICOMThumbnailWidget)));
            }else{
                logger.error("No thumbnail file " + thumbnail);
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::onStudyModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMThumbnailListWidget);

    QModelIndex studyIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        model->fetchMore(studyIndex);
        int seriesCount = model->rowCount(studyIndex);

        for(int i=0; i<seriesCount; i++){
            QModelIndex seriesIndex = studyIndex.child(i, 0);
            QModelIndex imageIndex = seriesIndex.child(0, 0);

            QString thumbnail = this->databaseDirectory +
                                    "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                    model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";

            if (QFile(thumbnail).exists()){
                ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(this->scrollAreaContentWidget);
                widget->setText( model->data(seriesIndex, Qt::DisplayRole).toString() );
                QPixmap pix(thumbnail);
                logger.debug("Setting pixmap to " + thumbnail);
                widget->setPixmap(pix);
                widget->setSourceIndex(seriesIndex);
                this->scrollAreaContentWidget->layout()->addWidget(widget);
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkDICOMThumbnailWidget)));
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SIGNAL(selected(ctkDICOMThumbnailWidget)));
            }else{
                logger.error("No thumbnail file " + thumbnail);
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::onSeriesModelSelected(const QModelIndex &index){
    Q_Q(ctkDICOMThumbnailListWidget);

    QModelIndex studyIndex = index.parent();
    QModelIndex seriesIndex = index;

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        model->fetchMore(seriesIndex);

        QString thumbnailPath = this->databaseDirectory +
                                "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
                                model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/";

        int imageCount = model->rowCount(seriesIndex);
        logger.debug(QString("Thumbs: %1").arg(imageCount));
        for (int i = 0 ; i < imageCount ; i++ )
        {
            QModelIndex imageIndex = seriesIndex.child(i,0);
            QString thumbnail = thumbnailPath + model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";
            if (QFile(thumbnail).exists())
            {
                ctkDICOMThumbnailWidget* widget = new ctkDICOMThumbnailWidget(this->scrollAreaContentWidget);
                QString widgetLabel = QString("Image %1").arg(i);
                widget->setText( widgetLabel );
                QPixmap pix(thumbnail);
                logger.debug("Setting pixmap to " + thumbnail);
                widget->setPixmap(pix);
                widget->setSourceIndex(imageIndex);
                this->scrollAreaContentWidget->layout()->addWidget(widget);
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkDICOMThumbnailWidget)));
                q->connect(widget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SIGNAL(selected(ctkDICOMThumbnailWidget)));
            }
            else
            {
                logger.error("No thumbnail file " + thumbnail);
            }
        }
    }
}

void ctkDICOMThumbnailListWidgetPrivate::clearAllThumbnails(){
    Q_Q(ctkDICOMThumbnailListWidget);

    // Remove previous displayed thumbnails
    QLayoutItem* item;
    while(item = this->scrollAreaContentWidget->layout()->takeAt(0)){
        ctkDICOMThumbnailWidget* thumbnailWidget = qobject_cast<ctkDICOMThumbnailWidget*>(item->widget());
        if(thumbnailWidget){
            q->disconnect(thumbnailWidget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkDICOMThumbnailWidget)));
            q->disconnect(thumbnailWidget, SIGNAL(selected(ctkDICOMThumbnailWidget)), q, SIGNAL(selected(ctkDICOMThumbnailWidget)));
        }
        item->widget()->deleteLater();
    }
}

//----------------------------------------------------------------------------
// ctkDICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::ctkDICOMThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
									   d_ptr(new ctkDICOMThumbnailListWidgetPrivate(this))
{
    Q_D(ctkDICOMThumbnailListWidget);

    d->setupUi(this);

    d->scrollAreaContentWidget->setLayout(new ctkFlowLayout);
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailListWidget::~ctkDICOMThumbnailListWidget()
{
    Q_D(ctkDICOMThumbnailListWidget);

    d->clearAllThumbnails();
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::setDatabaseDirectory(const QString &directory){
    Q_D(ctkDICOMThumbnailListWidget);

    d->databaseDirectory = directory;
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::selectThumbnail(int index){
    Q_D(ctkDICOMThumbnailListWidget);

    int count = d->scrollAreaContentWidget->layout()->count();

    logger.debug("Select thumbnail " + QVariant(index).toString() + " of " + QVariant(count).toString());

    if(index >= count)return;

    for(int i=0; i<count; i++){
        ctkDICOMThumbnailWidget* thumbnailWidget = qobject_cast<ctkDICOMThumbnailWidget*>(d->scrollAreaContentWidget->layout()->itemAt(i)->widget());
        if(i == index){
            thumbnailWidget->setSelected(true);
        }else{
            thumbnailWidget->setSelected(false);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::onModelSelected(const QModelIndex &index){
    Q_D(ctkDICOMThumbnailListWidget);

    d->clearAllThumbnails();

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model){
        QModelIndex index0 = index.sibling(index.row(), 0);

        if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::PatientType ){
            d->onPatientModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::StudyType ){
            d->onStudyModelSelected(index0);
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::SeriesType ){
            d->onSeriesModelSelected(index0);
        }
    }

    this->selectThumbnail(0);
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailListWidget::onThumbnailSelected(const ctkDICOMThumbnailWidget &widget){
    Q_D(ctkDICOMThumbnailListWidget);
    for(int i=0; i<d->scrollAreaContentWidget->layout()->count(); i++){
        ctkDICOMThumbnailWidget* thumbnailWidget = qobject_cast<ctkDICOMThumbnailWidget*>(d->scrollAreaContentWidget->layout()->itemAt(i)->widget());
        if(thumbnailWidget && (&widget != thumbnailWidget)){
            thumbnailWidget->setSelected(false);
        }
    }
}

