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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

//Qt includes
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QtSvg/QSvgRenderer>
#include <QTextItem>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMThumbnailGenerator.h"

// ctkDICOMWidgets includes
#include "ctkDICOMSeriesItemWidget.h"
#include "ui_ctkDICOMSeriesItemWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMSeriesItemWidget");

//----------------------------------------------------------------------------
class ctkDICOMSeriesItemWidgetPrivate: public Ui_ctkDICOMSeriesItemWidget
{
  Q_DECLARE_PUBLIC( ctkDICOMSeriesItemWidget );

protected:
  ctkDICOMSeriesItemWidget* const q_ptr;

public:
  ctkDICOMSeriesItemWidgetPrivate(ctkDICOMSeriesItemWidget& obj);
  ~ctkDICOMSeriesItemWidgetPrivate();

  void init();
  QString getDICOMCenterFrameFromInstances(QStringList instancesList);
  void createThumbnail(ctkJobDetail td);
  void drawModalityThumbnail();
  void drawThumbnail(const QString& file, int numberOfFrames);
  void drawTextWithShadow(QPainter *painter,
                          const QRect &r,
                          int flags,
                          const QString &text);
  void updateThumbnailProgressBar();

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;

  QString PatientID;
  QString SeriesItem;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString CentralFrameSOPInstanceUID;
  QString SeriesNumber;
  QString Modality;
  bool StopJobs;
  bool RaiseJobsPriority;
  bool IsCloud;
  bool IsLoaded;
  bool IsVisible;
  int ThumbnailSize;
  int NumberOfDownloads;
  QImage ThumbnailImage;
  bool isThumbnailDocument;
};

//----------------------------------------------------------------------------
// ctkDICOMSeriesItemWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidgetPrivate::ctkDICOMSeriesItemWidgetPrivate(ctkDICOMSeriesItemWidget& obj)
  : q_ptr(&obj)
{
  this->PatientID = "";
  this->SeriesItem = "";
  this->StudyInstanceUID = "";
  this->SeriesInstanceUID = "";
  this->CentralFrameSOPInstanceUID = "";
  this->SeriesNumber = "";
  this->Modality = "";

  this->IsCloud = false;
  this->IsLoaded = false;
  this->IsVisible = false;
  this->StopJobs = false;
  this->RaiseJobsPriority = false;
  this->isThumbnailDocument = false;
  this->ThumbnailSize = 300;
  this->NumberOfDownloads = 0;

  this->DicomDatabase = nullptr;
  this->Scheduler = nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidgetPrivate::~ctkDICOMSeriesItemWidgetPrivate()
{
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::init()
{
  Q_Q(ctkDICOMSeriesItemWidget);
  this->setupUi(q);

  this->SeriesThumbnail->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidgetPrivate::getDICOMCenterFrameFromInstances(QStringList instancesList)
{
  if (!this->DicomDatabase)
    {
    logger.error("getDICOMCenterFrameFromInstances failed, no DICOM Database has been set. \n");
    return "";
    }

  if (instancesList.count() == 0)
    {
    return "";
    }

  // NOTE: we sort by the instance number.
  // We could sort for 3D spatial values (ImagePatientPosition and ImagePatientOrientation),
  // plus time information (for 4D datasets). However, this would require additional metadata fetching and logic, which can slow down.
  QMap<int, QString> DICOMInstances;
  foreach (QString instanceItem, instancesList)
    {
    int instanceNumber = 0;
    QString instanceNumberString = this->DicomDatabase->instanceValue(instanceItem, "0020,0013");

    if (instanceNumberString != "")
      {
      instanceNumber = instanceNumberString.toInt();
      }

    DICOMInstances[instanceNumber] = instanceItem;
    }

  if (DICOMInstances.count() == 1)
    {
    return instancesList[0];
    }

  QList<int> keys = DICOMInstances.keys();
  std::sort(keys.begin(), keys.end());

  int centerFrameIndex = floor(keys.count() / 2);
  if (keys.count() <= centerFrameIndex)
    {
    return instancesList[0];
    }

  int centerInstanceNumber = keys[centerFrameIndex];

  return DICOMInstances[centerInstanceNumber];
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::createThumbnail(ctkJobDetail td)
{
  if (!this->DicomDatabase)
    {
    logger.error("importFiles failed, no DICOM Database has been set. \n");
    return;
    }

  ctkDICOMJobResponseSet::JobType typeOfJob = ctkDICOMJobResponseSet::JobType::None;
  QString jobSopInstanceUID;
  if (!td.JobUID.isEmpty())
    {
    jobSopInstanceUID = td.SOPInstanceUID;
    typeOfJob = td.TypeOfJob;
    }

  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();
  if (numberOfFrames == 0)
    {
    this->drawModalityThumbnail();
    return;
    }

  QStringList filesList = this->DicomDatabase->filesForSeries(this->SeriesInstanceUID);
  filesList.removeAll(QString(""));
  int numberOfFiles = filesList.count();
  QStringList urlsList = this->DicomDatabase->urlsForSeries(this->SeriesInstanceUID);
  filesList.removeAll(QString(""));
  int numberOfUrls = urlsList.count();
  if (!this->IsCloud && numberOfFrames > 0 && numberOfUrls > 0 && numberOfFiles < numberOfFrames)
    {
    this->IsCloud = true;
    this->drawModalityThumbnail();
    }
  else if (this->IsCloud && numberOfFrames > 0 && numberOfFiles == numberOfFrames)
    {
    this->IsCloud = false;
    this->SeriesThumbnail->operationProgressBar()->hide();
    }

  if (!this->IsCloud)
    {
    if(this->DicomDatabase->visibleSeries().contains(this->SeriesInstanceUID))
      {
      this->IsVisible = true;
      }
    else if (this->DicomDatabase->loadedSeries().contains(this->SeriesInstanceUID))
      {
      this->IsLoaded = true;
      }
    else
      {
      this->IsVisible = false;
      this->IsLoaded = false;
      }
    }

  QString file;
  if (this->CentralFrameSOPInstanceUID.isEmpty())
    {
    this->CentralFrameSOPInstanceUID = this->getDICOMCenterFrameFromInstances(instancesList);
    file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);

    // Since getDICOMCenterFrameFromInstances is based on the sorting of the instance number,
    // which is not always reliable, it could fail to get the right central frame.
    // In these cases, we check if a frame has been already fetched and we use the first found one.
    if (file.isEmpty() && numberOfFiles < numberOfFrames)
      {
      foreach(QString newFile, filesList)
        {
        if (file.isEmpty())
          {
          continue;
          }

        file = newFile;
        this->CentralFrameSOPInstanceUID = this->DicomDatabase->instanceForFile(file);
        break;
        }
      }
    }
  else
    {
    file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
    }

  if (!this->StopJobs &&
      this->Scheduler &&
      this->Scheduler->getNumberOfQueryRetrieveServers() > 0)
    {
    // Get file for thumbnail
    if (file.isEmpty() &&
        this->IsCloud &&
        (typeOfJob == ctkDICOMJobResponseSet::JobType::None ||
         typeOfJob == ctkDICOMJobResponseSet::JobType::QueryInstances))
      {
      this->Scheduler->retrieveSOPInstance(this->PatientID,
                                           this->StudyInstanceUID,
                                           this->SeriesInstanceUID,
                                           this->CentralFrameSOPInstanceUID,
                                           this->RaiseJobsPriority ? QThread::HighestPriority : QThread::HighPriority);

      return;
      }

    // Get series
    if (numberOfFrames > 1 &&
        this->IsCloud &&
        ((jobSopInstanceUID == this->CentralFrameSOPInstanceUID &&
        (typeOfJob == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
         typeOfJob == ctkDICOMJobResponseSet::JobType::StoreSOPInstance)) ||
         (typeOfJob == ctkDICOMJobResponseSet::JobType::None ||
         typeOfJob == ctkDICOMJobResponseSet::JobType::QueryInstances)))
      {
      this->Scheduler->retrieveSeries(this->PatientID,
                                      this->StudyInstanceUID,
                                      this->SeriesInstanceUID,
                                      this->RaiseJobsPriority ? QThread::HighestPriority : QThread::LowPriority);
      }
    }

  file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
  if ((jobSopInstanceUID.isEmpty() ||
       jobSopInstanceUID == this->CentralFrameSOPInstanceUID ||
       typeOfJob == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
       typeOfJob == ctkDICOMJobResponseSet::JobType::StoreSOPInstance) &&
      !file.isEmpty())
    {
    this->drawThumbnail(file, numberOfFrames);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::drawModalityThumbnail()
{
  if (!this->DicomDatabase)
    {
    logger.error("drawThumbnail failed, no DICOM Database has been set. \n");
    return;
    }

  int margin = 10;
  int fontSize = 40;

  QPixmap resultPixmap(this->ThumbnailSize, this->ThumbnailSize);
  resultPixmap.fill(Qt::transparent);
  ctkDICOMThumbnailGenerator thumbnailGenerator;
  thumbnailGenerator.setWidth(this->ThumbnailSize);
  thumbnailGenerator.setHeight(this->ThumbnailSize);

  QImage thumbnailImage;
  QPainter painter;

  thumbnailGenerator.generateBlankThumbnail(thumbnailImage, Qt::white);
  resultPixmap = QPixmap::fromImage(thumbnailImage);
  if (painter.begin(&resultPixmap))
    {
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = resultPixmap.rect();
    painter.setFont(QFont("Arial", fontSize, QFont::Bold));
    this->drawTextWithShadow(&painter, rect.adjusted(margin, margin, margin, margin), Qt::AlignCenter, this->Modality);
    painter.end();
    }

  this->SeriesThumbnail->setPixmap(resultPixmap);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::drawThumbnail(const QString &file, int numberOfFrames)
{
  if (!this->DicomDatabase)
    {
    logger.error("drawThumbnail failed, no DICOM Database has been set. \n");
    return;
    }

  int margin = 10;
  int fontSize = 12;
  if (!this->SeriesThumbnail->text().isEmpty())
    {
    margin = 5;
    fontSize = 14;
    }

  QPixmap resultPixmap(this->ThumbnailSize, this->ThumbnailSize);
  resultPixmap.fill(Qt::transparent);
  ctkDICOMThumbnailGenerator thumbnailGenerator;
  thumbnailGenerator.setWidth(this->ThumbnailSize);
  thumbnailGenerator.setHeight(this->ThumbnailSize);
  bool thumbnailGenerated = true;
  bool emptyThumbnailGenerated = false;
  QPainter painter;
  if (this->ThumbnailImage.width() != this->ThumbnailSize)
    {
    if (!thumbnailGenerator.generateThumbnail(file, this->ThumbnailImage))
      {
      thumbnailGenerated = false;
      emptyThumbnailGenerated = true;
      this->isThumbnailDocument = true;
      thumbnailGenerator.generateBlankThumbnail(this->ThumbnailImage, Qt::white);
      resultPixmap = QPixmap::fromImage(this->ThumbnailImage);
      if (painter.begin(&resultPixmap))
        {
        painter.setRenderHint(QPainter::Antialiasing);
        QSvgRenderer renderer(QString(":Icons/text_document.svg"));
        renderer.render(&painter);
        painter.end();
        }
      }
    }

  if (thumbnailGenerated && !this->isThumbnailDocument)
    {
    if (painter.begin(&resultPixmap))
      {
      painter.setRenderHint(QPainter::Antialiasing);
      QRect rect = resultPixmap.rect();
      painter.setFont(QFont("Arial", fontSize, QFont::Bold));
      int x = int((rect.width() / 2) - (this->ThumbnailImage.rect().width() / 2));
      int y = int((rect.height() / 2) - (this->ThumbnailImage.rect().height() / 2));
      painter.drawPixmap(x, y, QPixmap::fromImage(this->ThumbnailImage));
      QString topLeft = ctkDICOMSeriesItemWidget::tr("Series: %1\n%2").arg(this->SeriesNumber).arg(this->Modality);
      this->drawTextWithShadow(&painter, rect.adjusted(margin, margin, margin, margin), Qt::AlignTop | Qt::AlignLeft, topLeft);
      QString bottomLeft = ctkDICOMSeriesItemWidget::tr("N.frames: %1").arg(numberOfFrames);
      this->drawTextWithShadow(&painter, rect.adjusted(margin, -margin, margin, -margin), Qt::AlignBottom | Qt::AlignLeft, bottomLeft);
      QString rows = this->DicomDatabase->instanceValue(this->CentralFrameSOPInstanceUID, "0028,0010");
      QString columns = this->DicomDatabase->instanceValue(this->CentralFrameSOPInstanceUID, "0028,0011");
      QString bottomRight = rows + "x" + columns;
      this->drawTextWithShadow(&painter, rect.adjusted(-margin, -margin, -margin, -margin), Qt::AlignBottom | Qt::AlignRight, bottomRight);
      QSvgRenderer renderer;

      if (this->IsCloud)
        {
        if (this->NumberOfDownloads > 0)
          {
          renderer.load(QString(":Icons/downloading.svg"));
          }
          else
          {
          renderer.load(QString(":Icons/cloud.svg"));
          }
        }
      else if (this->IsVisible)
        {
        renderer.load(QString(":Icons/visible.svg"));
        }
      else if (this->IsLoaded)
        {
        renderer.load(QString(":Icons/loaded.svg"));
        }

      QPoint topRight = rect.topRight();
      QRectF bounds(topRight.x() - 48 - margin, topRight.y() + margin, 48, 48);
      renderer.render(&painter, bounds);
      painter.end();
      }
    }

  if ((thumbnailGenerated && !this->isThumbnailDocument) || emptyThumbnailGenerated)
    {
    this->SeriesThumbnail->setPixmap(resultPixmap);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::drawTextWithShadow(QPainter *painter,
                                                         const QRect &r,
                                                         int flags, const
                                                         QString &text)
{
  painter->setPen(Qt::darkGray);
  painter->drawText(r.adjusted(1, 1, 1, 1), flags, text);
  painter->setPen(QColor(Qt::gray));
  painter->drawText(r.adjusted(2, 2, 2, 2), flags, text);
  painter->setPen(QPen(QColor(41, 121, 255)));
  painter->drawText(r, flags, text);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateThumbnailProgressBar()
{
  if (!this->IsCloud)
    {
    return;
    }

  this->NumberOfDownloads++;
  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();
  float percentageOfInstancesOnLocal = float(this->NumberOfDownloads) / numberOfFrames;
  int progress = ceil(percentageOfInstancesOnLocal * 100);
  progress = progress > 100 ? 100 : progress;
  this->SeriesThumbnail->setOperationProgress(progress);
  if (this->NumberOfDownloads == 1)
    {
    this->SeriesThumbnail->operationProgressBar()->show();
    // change icons
    QString file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
    if (!file.isEmpty())
      {
      this->drawThumbnail(file, numberOfFrames);
      }
    }
}

//----------------------------------------------------------------------------
// ctkDICOMSeriesItemWidget methods

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidget::ctkDICOMSeriesItemWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMSeriesItemWidgetPrivate(*this))
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidget::~ctkDICOMSeriesItemWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setSeriesItem(const QString &seriesItem)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->SeriesItem = seriesItem;
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::seriesItem() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->SeriesItem;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setPatientID(const QString &patientID)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->PatientID = patientID;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::patientID() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->PatientID;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setStudyInstanceUID(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->StudyInstanceUID = studyInstanceUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::studyInstanceUID() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->StudyInstanceUID;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setSeriesInstanceUID(const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->SeriesInstanceUID = seriesInstanceUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::seriesInstanceUID() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->SeriesInstanceUID;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setSeriesNumber(const QString& seriesNumber)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->SeriesNumber = seriesNumber;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::seriesNumber() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->SeriesNumber;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setModality(const QString& modality)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->Modality = modality;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::modality() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->Modality;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setSeriesDescription(const QString& seriesDescription)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->SeriesThumbnail->setText(seriesDescription);
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::seriesDescription() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->SeriesThumbnail->text();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setStopJobs(const bool &stopJobs)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->StopJobs = stopJobs;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::stopJobs() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->StopJobs;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setRaiseJobsPriority(const bool &raiseJobsPriority)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->RaiseJobsPriority = raiseJobsPriority;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::raiseJobsPriority() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->RaiseJobsPriority;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::isCloud() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->IsCloud;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::IsLoaded() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->IsLoaded;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::IsVisible() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->IsVisible;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setThumbnailSize(int thumbnailSize)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->ThumbnailSize = thumbnailSize;
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesItemWidget::thumbnailSize() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->ThumbnailSize;
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMSeriesItemWidget::scheduler()const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMSeriesItemWidget::schedulerShared()const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (d->Scheduler)
    {
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                       this, SLOT(updateGUIFromScheduler(QVariant)));
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                        this, SLOT(updateSeriesProgressBar(QVariant)));
    }

  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);

  if (d->Scheduler)
    {
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateGUIFromScheduler(QVariant)));
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateSeriesProgressBar(QVariant)));
    }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (d->Scheduler)
    {
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                       this, SLOT(updateGUIFromScheduler(QVariant)));
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                       this, SLOT(updateSeriesProgressBar(QVariant)));
    }

  d->Scheduler = scheduler;

  if (d->Scheduler)
    {
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateGUIFromScheduler(QVariant)));
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateSeriesProgressBar(QVariant)));
    }
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMSeriesItemWidget::dicomDatabase()const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMSeriesItemWidget::dicomDatabaseShared()const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setDicomDatabase(ctkDICOMDatabase& dicomDatabase)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&dicomDatabase, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->DicomDatabase = dicomDatabase;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::generateInstances()
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (!d->DicomDatabase)
    {
    logger.error("generateInstances failed, no DICOM Database has been set. \n");
    return;
    }

  ctkJobDetail td;
  d->createThumbnail(td);
  QStringList instancesList = d->DicomDatabase->instancesForSeries(d->SeriesInstanceUID);
  if (!d->StopJobs &&
      instancesList.count() == 0 &&
      d->Scheduler &&
      d->Scheduler->getNumberOfQueryRetrieveServers() > 0)
    {
    d->Scheduler->queryInstances(d->PatientID,
                                 d->StudyInstanceUID,
                                 d->SeriesInstanceUID,
                                 d->RaiseJobsPriority ? QThread::HighestPriority : QThread::NormalPriority);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::updateGUIFromScheduler(QVariant data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkJobDetail td = data.value<ctkJobDetail>();
  if (td.JobUID.isEmpty() ||
      (td.TypeOfJob != ctkDICOMJobResponseSet::JobType::QueryInstances &&
       td.TypeOfJob != ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
       td.TypeOfJob != ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance&&
       td.TypeOfJob != ctkDICOMJobResponseSet::JobType::StoreSOPInstance) ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
    {
    return;
    }

  d->createThumbnail(td);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::updateSeriesProgressBar(QVariant data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkJobDetail td = data.value<ctkJobDetail>();
  if (td.JobUID.isEmpty() ||
      (td.TypeOfJob != ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
       td.TypeOfJob != ctkDICOMJobResponseSet::JobType::StoreSOPInstance) ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
    {
    return;
    }

  d->updateThumbnailProgressBar();
}
