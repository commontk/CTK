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

// Qt includes
#include <QDebug>
#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QtSvg/QSvgRenderer>
#include <QTextItem>

// CTK includes
#include <ctkLogger.h>
#include <ctkPushButton.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMJob.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMThumbnailGenerator.h"

// ctkDICOMWidgets includes
#include "ctkDICOMSeriesItemWidget.h"
#include "ui_ctkDICOMSeriesItemWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMSeriesItemWidget");

//----------------------------------------------------------------------------
class ctkDICOMSeriesItemWidgetPrivate : public Ui_ctkDICOMSeriesItemWidget
{
  Q_DECLARE_PUBLIC(ctkDICOMSeriesItemWidget);

protected:
  ctkDICOMSeriesItemWidget* const q_ptr;

public:
  ctkDICOMSeriesItemWidgetPrivate(ctkDICOMSeriesItemWidget& obj);
  ~ctkDICOMSeriesItemWidgetPrivate();

  void init();
  QString getDICOMCenterFrameFromInstances(QStringList instancesList);
  void createThumbnail(ctkDICOMJobDetail td, bool queryRetrieve = true);
  void drawModalityThumbnail();
  void drawThumbnail(const QString& file, int numberOfFrames);
  void drawTextWithShadow(QPainter *painter,
                          const QFont &font,
                          int x,
                          int y,
                          Qt::Alignment alignment,
                          const QString &text);
  void updateThumbnailProgressBar();
  void resetOperationProgressBar();
  void updateUIOnStarted();
  void updateUIOnFailed();
  void updateUIOnFinished();
  void connectScheduler();
  void disconnectScheduler();

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QStringList AllowedServers;

  QString PatientID;
  QString SeriesItem;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString CentralFrameSOPInstanceUID;
  QString SeriesNumber;
  QString Modality;
  QString ReferenceInserterJobUID;
  bool StopJobs;
  bool RaiseJobsPriority;
  bool IsCloud;
  bool RetrieveFailed;
  bool IsLoaded;
  bool IsVisible;
  int ThumbnailSizePixel;
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
  this->ReferenceInserterJobUID = "";

  this->IsCloud = false;
  this->RetrieveFailed = false;
  this->IsLoaded = false;
  this->IsVisible = false;
  this->StopJobs = false;
  this->RaiseJobsPriority = false;
  this->isThumbnailDocument = false;
  this->ThumbnailSizePixel = 200;
  this->NumberOfDownloads = 0;

  this->AllowedServers = QStringList();

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
  this->SeriesThumbnail->textPushButton()->setElideMode(Qt::ElideRight);
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidgetPrivate::getDICOMCenterFrameFromInstances(QStringList instancesList)
{
  if (!this->DicomDatabase)
  {
    logger.error("getDICOMCenterFrameFromInstances failed, no DICOM Database has been set. \n");
    return "";
  }

  int numberOfInstances = instancesList.count();
  if (numberOfInstances == 0)
  {
    return "";
  }

  // NOTE: we sort by the instance number.
  // We could sort for 3D spatial values (ImagePatientPosition and ImagePatientOrientation),
  // plus time information (for 4D datasets). However, this would require additional metadata fetching and logic, which can slow down.
  QMap<int, QString> sortedInstancesMap;
  foreach (QString instanceItem, instancesList)
  {
    int instanceNumber = 0;
    QString instanceNumberString = this->DicomDatabase->instanceValue(instanceItem, "0020,0013");

    if (instanceNumberString != "")
    {
      instanceNumber = instanceNumberString.toInt();
    }

    sortedInstancesMap[instanceNumber] = instanceItem;
  }

  if (sortedInstancesMap.count() < 2)
  {
    return instancesList[floor(numberOfInstances / 2)];
  }

  QList<int> keys = sortedInstancesMap.keys();
  std::sort(keys.begin(), keys.end());

  int centerInstanceIndex = floor(keys.count() / 2);
  int centerInstanceNumber = keys[centerInstanceIndex];
  return sortedInstancesMap[centerInstanceNumber];
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::createThumbnail(ctkDICOMJobDetail td, bool queryRetrieve)
{
  if (!this->DicomDatabase)
  {
    logger.error("importFiles failed, no DICOM Database has been set. \n");
    return;
  }

  ctkDICOMJobResponseSet::JobType jobType = ctkDICOMJobResponseSet::JobType::None;
  QString jobSopInstanceUID;
  if (!td.JobUID.isEmpty())
  {
    jobSopInstanceUID = td.SOPInstanceUID;
    jobType = td.JobType;
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
  urlsList.removeAll(QString(""));
  int numberOfUrls = urlsList.count();
  bool renderThumbnail = false;
  if (!this->IsCloud && numberOfFrames > 0 && numberOfUrls > 0 && numberOfFiles < numberOfFrames)
  {
    this->IsCloud = true;
    this->drawModalityThumbnail();
  }

  if (this->IsCloud && numberOfFrames == 1 && numberOfFiles == numberOfFrames)
  {
    this->IsCloud = false;
    renderThumbnail = true;
  }

  if (this->RetrieveFailed)
  {
    renderThumbnail = true;
  }

  if (!this->IsCloud)
  {
    if (this->DicomDatabase->visibleSeries().contains(this->SeriesInstanceUID))
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
      foreach (QString newFile, filesList)
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
      this->Scheduler->queryRetrieveServersCount() > 0)
  {
    // Get file for thumbnail
    if (file.isEmpty() &&
        (this->IsCloud || this->RetrieveFailed) &&
        (jobType == ctkDICOMJobResponseSet::JobType::None ||
         jobType == ctkDICOMJobResponseSet::JobType::QueryInstances))
    {
      if (queryRetrieve)
      {
        this->Scheduler->retrieveSOPInstance(this->PatientID,
                                             this->StudyInstanceUID,
                                             this->SeriesInstanceUID,
                                             this->CentralFrameSOPInstanceUID,
                                             this->RaiseJobsPriority ? QThread::HighestPriority : QThread::HighPriority,
                                             this->AllowedServers);
      }
      return;
    }

    // Get series
    if (numberOfFrames > 1 &&
        (this->IsCloud || this->RetrieveFailed) &&
        ((jobSopInstanceUID == this->CentralFrameSOPInstanceUID &&
        (jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
         jobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance)) ||
         (jobType == ctkDICOMJobResponseSet::JobType::None ||
         jobType == ctkDICOMJobResponseSet::JobType::QueryInstances)))
    {
      QList<QSharedPointer<ctkAbstractJob>> jobs =
        this->Scheduler->getJobsByDICOMUIDs({},
                                            {},
                                            {this->SeriesInstanceUID});
      if (jobs.count() == 0 && queryRetrieve)
      {
        this->Scheduler->retrieveSeries(this->PatientID,
                                        this->StudyInstanceUID,
                                        this->SeriesInstanceUID,
                                        this->RaiseJobsPriority ? QThread::HighestPriority : QThread::LowPriority,
                                        this->AllowedServers);
      }
    }
  }

  file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
  if (file.isEmpty())
  {
    return;
  }

  if (jobSopInstanceUID.isEmpty() ||
      ((jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        jobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance) &&
       jobSopInstanceUID == this->CentralFrameSOPInstanceUID) ||
      renderThumbnail)
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

  int textSize = floor(this->ThumbnailSizePixel / 7.);
  QFont font = this->SeriesThumbnail->font();
  font.setBold(true);
  font.setPixelSize(textSize);

  QPixmap resultPixmap(this->ThumbnailSizePixel, this->ThumbnailSizePixel);
  resultPixmap.fill(Qt::transparent);
  ctkDICOMThumbnailGenerator thumbnailGenerator;
  thumbnailGenerator.setWidth(this->ThumbnailSizePixel);
  thumbnailGenerator.setHeight(this->ThumbnailSizePixel);

  QImage thumbnailImage;
  QPainter painter;

  QColor backgroundColor = this->SeriesThumbnail->palette().color(QPalette::Normal, QPalette::Window);
  thumbnailGenerator.generateBlankThumbnail(thumbnailImage, backgroundColor);
  resultPixmap = QPixmap::fromImage(thumbnailImage);
  if (painter.begin(&resultPixmap))
  {
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = resultPixmap.rect();
    int x = int(rect.width() * 0.5);
    int y = int(rect.height() * 0.5);
    this->drawTextWithShadow(&painter, font, x, y, Qt::AlignCenter, this->Modality);
    painter.end();
  }

  this->SeriesThumbnail->setPixmap(resultPixmap);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::drawThumbnail(const QString& file, int numberOfFrames)
{
  if (!this->DicomDatabase)
  {
    logger.error("drawThumbnail failed, no DICOM Database has been set. \n");
    return;
  }

  int margin = floor(this->ThumbnailSizePixel / 60.);
  int iconSize = floor(this->ThumbnailSizePixel / 6.);
  int textSize = floor(this->ThumbnailSizePixel / 12.);
  QFont font = this->SeriesThumbnail->font();
  font.setBold(true);
  font.setPixelSize(textSize);

  QPixmap resultPixmap(this->ThumbnailSizePixel, this->ThumbnailSizePixel);
  resultPixmap.fill(Qt::transparent);
  ctkDICOMThumbnailGenerator thumbnailGenerator;
  thumbnailGenerator.setWidth(this->ThumbnailSizePixel);
  thumbnailGenerator.setHeight(this->ThumbnailSizePixel);
  bool thumbnailGenerated = true;
  bool emptyThumbnailGenerated = false;
  QPainter painter;
  if (this->ThumbnailImage.width() != this->ThumbnailSizePixel)
  {
    if (!thumbnailGenerator.generateThumbnail(file, this->ThumbnailImage))
    {
      thumbnailGenerated = false;
      emptyThumbnailGenerated = true;
      this->isThumbnailDocument = true;
      QColor backgroundColor = this->SeriesThumbnail->palette().color(QPalette::Normal, QPalette::Window);
      thumbnailGenerator.generateBlankThumbnail(this->ThumbnailImage, backgroundColor);
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
      painter.setFont(font);
      int x = int((rect.width() * 0.5) - (this->ThumbnailImage.rect().width() * 0.5));
      int y = int((rect.height() * 0.5) - (this->ThumbnailImage.rect().height() * 0.5));
      painter.drawPixmap(x, y, QPixmap::fromImage(this->ThumbnailImage));

      QString topLeftString = ctkDICOMSeriesItemWidget::tr("Series: %1\n%2").arg(this->SeriesNumber).arg(this->Modality);
      this->drawTextWithShadow(&painter, font, margin, margin, Qt::AlignTop | Qt::AlignLeft, topLeftString);
      QString rows = this->DicomDatabase->instanceValue(this->CentralFrameSOPInstanceUID, "0028,0010");
      QString columns = this->DicomDatabase->instanceValue(this->CentralFrameSOPInstanceUID, "0028,0011");
      QString bottomLeftString = rows + "x" + columns + "x" + QString::number(numberOfFrames);
      this->drawTextWithShadow(&painter, font, margin, rect.height() - margin,
                               Qt::AlignBottom | Qt::AlignLeft, bottomLeftString);
      QSvgRenderer renderer;

      if (this->RetrieveFailed)
      {
        renderer.load(QString(":Icons/error_red.svg"));
      }
      else if (this->IsCloud)
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
      QRectF bounds(topRight.x() - iconSize - margin, topRight.y() + margin, iconSize, iconSize);
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
                                                         const QFont &font,
                                                         int x,
                                                         int y,
                                                         Qt::Alignment alignment,
                                                         const QString &text)
{
  QColor textColor(60, 164, 255, 225);
  QGraphicsDropShadowEffect* dropShadowEffect = new QGraphicsDropShadowEffect;
  dropShadowEffect->setXOffset(1);
  dropShadowEffect->setYOffset(1);
  dropShadowEffect->setBlurRadius(1);
  dropShadowEffect->setColor(Qt::gray);
  QLabel textLabel;
  textLabel.setObjectName("ctkDrawTextWithShadowQLabel");
  QPalette palette = textLabel.palette();
  palette.setColor(QPalette::WindowText, textColor);
  textLabel.setPalette(palette);
  textLabel.setFont(font);
  textLabel.setGraphicsEffect(dropShadowEffect);
  textLabel.setText(text);
  QPixmap textPixMap = textLabel.grab();
  QRect rect = textPixMap.rect();
  int textWidth = rect.width();
  int textHeight = rect.height();

  if (alignment == Qt::AlignCenter)
  {
    painter->drawPixmap(x - textWidth * 0.5, y - textHeight * 0.5, textPixMap);
  }
  else if (alignment == (Qt::AlignTop | Qt::AlignLeft))
  {
    painter->drawPixmap(x, y, textPixMap);
  }
  else if (alignment == Qt::AlignTop)
  {
    painter->drawPixmap(x - textWidth * 0.5, y, textPixMap);
  }
  else if (alignment == (Qt::AlignTop | Qt::AlignRight))
  {
    painter->drawPixmap(x - textWidth, y, textPixMap);
  }
  else if (alignment == (Qt::AlignHCenter | Qt::AlignLeft))
  {
    painter->drawPixmap(x, y - textHeight * 0.5, textPixMap);
  }
  else if (alignment == (Qt::AlignHCenter | Qt::AlignRight))
  {
    painter->drawPixmap(x - textWidth, y - textHeight * 0.5, textPixMap);
  }
  else if (alignment == (Qt::AlignBottom | Qt::AlignLeft))
  {
    painter->drawPixmap(x, y - textHeight, textPixMap);
  }
  else if (alignment == Qt::AlignBottom)
  {
    painter->drawPixmap(x - textWidth * 0.5, y - textHeight, textPixMap);
  }
  else if (alignment == (Qt::AlignBottom | Qt::AlignRight))
  {
    painter->drawPixmap(x - textWidth, y - textHeight, textPixMap);
  }
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
void ctkDICOMSeriesItemWidgetPrivate::resetOperationProgressBar()
{
  this->NumberOfDownloads = 0;
  this->SeriesThumbnail->setOperationProgress(0);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateUIOnStarted()
{
  this->ReferenceInserterJobUID = "";
  this->RetrieveFailed = false;
  this->resetOperationProgressBar();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateUIOnFailed()
{
  this->RetrieveFailed = true;
  this->ReferenceInserterJobUID = "";

  QString file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
  if (file.isEmpty())
  {
    return;
  }

  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();

  this->drawThumbnail(file, numberOfFrames);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateUIOnFinished()
{
  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();

  QStringList filesList = this->DicomDatabase->filesForSeries(this->SeriesInstanceUID);
  filesList.removeAll(QString(""));
  int numberOfFiles = filesList.count();

  if (numberOfFrames > 0 && numberOfFiles < numberOfFrames)
  {
    this->RetrieveFailed = true;
    this->IsCloud = false;
    this->SeriesThumbnail->operationProgressBar()->hide();
  }
  else if (numberOfFrames > 0 && numberOfFiles == numberOfFrames)
  {
    this->IsCloud = false;
    this->SeriesThumbnail->operationProgressBar()->hide();
  }

  QString file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
  if (file.isEmpty())
  {
    return;
  }

  this->drawThumbnail(file, numberOfFrames);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::connectScheduler()
{
  Q_Q(ctkDICOMSeriesItemWidget);
  QObject::connect(this->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                   q, SLOT(updateGUIFromScheduler(QVariant)));
  QObject::connect(this->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                   q, SLOT(updateSeriesProgressBar(QVariant)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobStarted(QVariant)),
                   q, SLOT(onJobStarted(QVariant)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobCanceled(QVariant)),
                   q, SLOT(onJobCanceled(QVariant)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                   q, SLOT(onJobFailed(QVariant)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                   q, SLOT(onJobFinished(QVariant)));
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::disconnectScheduler()
{
  Q_Q(ctkDICOMSeriesItemWidget);
  QObject::disconnect(this->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                      q, SLOT(updateGUIFromScheduler(QVariant)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                      q, SLOT(updateSeriesProgressBar(QVariant)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobStarted(QVariant)),
                      q, SLOT(onJobStarted(QVariant)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobCanceled(QVariant)),
                      q, SLOT(onJobCanceled(QVariant)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobFailed(QVariant)),
                      q, SLOT(onJobFailed(QVariant)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobFinished(QVariant)),
                      q, SLOT(onJobFinished(QVariant)));
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

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QStringList&, setAllowedServers, AllowedServers);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QStringList, allowedServers, AllowedServers);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setSeriesItem, SeriesItem);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, seriesItem, SeriesItem);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setPatientID, PatientID);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, patientID, PatientID);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setStudyInstanceUID, StudyInstanceUID);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, studyInstanceUID, StudyInstanceUID);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setSeriesInstanceUID, SeriesInstanceUID);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, seriesInstanceUID, SeriesInstanceUID);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setSeriesNumber, SeriesNumber);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, seriesNumber, SeriesNumber);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, const QString&, setModality, Modality);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, modality, Modality);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, bool, setStopJobs, StopJobs);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, stopJobs, StopJobs);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, bool, setRaiseJobsPriority, RaiseJobsPriority);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, raiseJobsPriority, RaiseJobsPriority);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isCloud, IsCloud);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isLoaded, IsLoaded);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isVisible, IsVisible);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, bool, setRetrieveFailed, RetrieveFailed);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, retrieveFailed, RetrieveFailed);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, int, setThumbnailSizePixel, ThumbnailSizePixel);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, int, thumbnailSizePixel, ThumbnailSizePixel);

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setSeriesDescription(const QString& seriesDescription)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->SeriesThumbnail->setText(seriesDescription);
  d->SeriesThumbnail->textPushButton()->setToolTip(seriesDescription);
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesItemWidget::seriesDescription() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->SeriesThumbnail->text();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::forceRetrieve()
{
  Q_D(ctkDICOMSeriesItemWidget);

  d->IsCloud = true;
  d->RetrieveFailed = false;
  d->StopJobs = false;
  ctkDICOMJobDetail td;
  d->createThumbnail(td);
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMSeriesItemWidget::scheduler() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMSeriesItemWidget::schedulerShared() const
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
    d->disconnectScheduler();
  }

  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);

  if (d->Scheduler)
  {
    d->connectScheduler();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (d->Scheduler)
  {
    d->disconnectScheduler();
  }

  d->Scheduler = scheduler;

  if (d->Scheduler)
  {
    d->connectScheduler();
  }
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMSeriesItemWidget::dicomDatabase() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMSeriesItemWidget::dicomDatabaseShared() const
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
void ctkDICOMSeriesItemWidget::generateInstances(bool queryRetrieve)
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (!d->DicomDatabase)
  {
    logger.error("generateInstances failed, no DICOM Database has been set. \n");
    return;
  }

  ctkDICOMJobDetail td;
  d->createThumbnail(td, queryRetrieve);
  QStringList instancesList = d->DicomDatabase->instancesForSeries(d->SeriesInstanceUID);
  if (queryRetrieve && !d->StopJobs &&
      instancesList.count() == 0 &&
      d->Scheduler &&
      d->Scheduler->queryRetrieveServersCount() > 0)
  {
    d->Scheduler->queryInstances(d->PatientID,
                                 d->StudyInstanceUID,
                                 d->SeriesInstanceUID,
                                 d->RaiseJobsPriority ? QThread::HighestPriority : QThread::NormalPriority,
                                 d->AllowedServers);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      (td.JobType != ctkDICOMJobResponseSet::JobType::QueryInstances &&
       td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance&&
       td.JobType != ctkDICOMJobResponseSet::JobType::StoreSOPInstance) ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->createThumbnail(td);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::updateSeriesProgressBar(const QVariant& data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      (td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
       td.JobType != ctkDICOMJobResponseSet::JobType::StoreSOPInstance) ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->updateThumbnailProgressBar();

  if (td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance &&
      d->ReferenceInserterJobUID == "StorageListener")
  {
    d->updateUIOnFinished();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobStarted(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if (td.JobUID.isEmpty() ||
      td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->updateUIOnStarted();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobCanceled(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->updateUIOnFailed();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobFailed(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
      td.StudyInstanceUID != d->StudyInstanceUID ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->updateUIOnFailed();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobFinished(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if (!td.JobUID.isEmpty() &&
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
      td.StudyInstanceUID == d->StudyInstanceUID &&
      td.SeriesInstanceUID == d->SeriesInstanceUID)
  {
    if (td.ReferenceInserterJobUID.isEmpty())
    {
      d->ReferenceInserterJobUID = "StorageListener";
    }
    else
    {
      d->ReferenceInserterJobUID = td.ReferenceInserterJobUID;
    }
    return;
  }

  if (td.JobUID.isEmpty() ||
      d->ReferenceInserterJobUID.isEmpty() ||
      td.JobType != ctkDICOMJobResponseSet::JobType::Inserter ||
      td.JobUID != d->ReferenceInserterJobUID)
  {
    return;
  }

  d->ReferenceInserterJobUID = "";
  d->updateUIOnFinished();
}
