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
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

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
  void createThumbnail(ctkDICOMJobDetail td);
  QImage drawModalityThumbnail();
  void drawThumbnail(const QString& dicomFilePath,
                     const QString& patientID,
                     const QString& studyInstanceUID,
                     const QString& seriesInstanceUID,
                     const QString& sopInstanceUID,
                     const QString& modality,
                     int numberOfFrames);
  void drawTextWithShadow(QPainter *painter,
                          const QFont &font,
                          int x,
                          int y,
                          Qt::Alignment alignment,
                          const QString &text);
  void updateThumbnailProgressBar();
  void resetOperationProgressBar();
  void updateRetrieveUIOnStarted();
  void updateRetrieveUIOnFailed();
  void updateRetrieveUIOnFinished();

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
  QString ReferenceSeriesInserterJobUID;
  QString ReferenceInstanceInserterJobUID;

  QString StoppedJobUID;

  bool StopJobs;
  bool RaiseJobsPriority;
  bool IsCloud;
  bool RetrieveFailed;
  bool RetrieveSeries;
  bool IsRetrieving;
  bool IsLoaded;
  bool IsVisible;
  bool ThumbnailIsGenerating;
  int ThumbnailSizePixel;
  int NumberOfDownloads;
  QImage ThumbnailImage;
  ctkDICOMThumbnailGenerator thumbnailGenerator;
  QHash<QString, QPair<QString, QString>> instanceRowsColumnsCache;
  QFont cachedFont;
  int cachedFontPixelSize;
  QPixmap cachedResultPixmap;
  int cachedPixmapSize;
  QMap<QString, QSharedPointer<QSvgRenderer>> svgRenderers;
  QScopedPointer<QLabel> shadowTextLabel;
  QScopedPointer<QGraphicsDropShadowEffect> shadowEffect;
  QFont shadowCachedFont;
  QString shadowCachedText;

  bool QueryOn;
  bool RetrieveOn;
};

//----------------------------------------------------------------------------
// ctkDICOMSeriesItemWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidgetPrivate::ctkDICOMSeriesItemWidgetPrivate(ctkDICOMSeriesItemWidget& obj)
  : q_ptr(&obj)
{
  this->IsCloud = false;
  this->RetrieveFailed = false;
  this->RetrieveSeries = false;
  this->IsRetrieving = false;
  this->IsLoaded = false;
  this->IsVisible = false;
  this->StopJobs = false;
  this->RaiseJobsPriority = false;
  this->ThumbnailIsGenerating = false;
  this->ThumbnailSizePixel = 200;
  this->NumberOfDownloads = 0;
  this->cachedFontPixelSize = -1;
  this->cachedPixmapSize = -1;

  this->AllowedServers = QStringList();

  this->DicomDatabase = nullptr;
  this->Scheduler = nullptr;

  this->QueryOn = true;
  this->RetrieveOn = true;
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
  this->SeriesThumbnail->setSelectedColor(QColor::Invalid);

  QObject::connect(this->SeriesThumbnail, SIGNAL(statusPushButtonClicked(bool)),
                 q, SLOT(onOperationStatusButtonClicked(bool)));
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
void ctkDICOMSeriesItemWidgetPrivate::createThumbnail(ctkDICOMJobDetail td)
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
         jobType == ctkDICOMJobResponseSet::JobType::QueryInstances) &&
        this->RetrieveOn)
    {
      this->Scheduler->retrieveSOPInstance(this->PatientID,
                                           this->StudyInstanceUID,
                                           this->SeriesInstanceUID,
                                           this->CentralFrameSOPInstanceUID,
                                           this->RaiseJobsPriority ? QThread::HighPriority : QThread::HighPriority,
                                           this->AllowedServers);
      this->RetrieveSeries = true;
      return;
    }

    // Get series
    if (numberOfFrames > 1 &&
        this->IsCloud &&
        this->RetrieveSeries &&
        this->RetrieveOn)
      {
      this->RetrieveSeries = false;
      this->Scheduler->retrieveSeries(this->PatientID,
                                      this->StudyInstanceUID,
                                      this->SeriesInstanceUID,
                                      this->RaiseJobsPriority ? QThread::HighPriority : QThread::LowPriority,
                                      this->AllowedServers);
    }
  }

  if (jobSopInstanceUID.isEmpty() ||
      ((jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        jobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
        jobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator) &&
       jobSopInstanceUID == this->CentralFrameSOPInstanceUID) ||
      renderThumbnail)
  {
    this->drawThumbnail(this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID),
                        this->PatientID,
                        this->StudyInstanceUID,
                        this->SeriesInstanceUID,
                        this->CentralFrameSOPInstanceUID,
                        this->Modality,
                        numberOfFrames);
  }
}

//----------------------------------------------------------------------------
QImage ctkDICOMSeriesItemWidgetPrivate::drawModalityThumbnail()
{
  Q_Q(ctkDICOMSeriesItemWidget);

  qreal scalingFactor = q->devicePixelRatioF();
  int scaledThumbnailSizePixel = this->ThumbnailSizePixel * scalingFactor;
  int textSize = floor(scaledThumbnailSizePixel / 7.);
  QFont font = this->SeriesThumbnail->font();
  font.setBold(true);
  font.setPixelSize(textSize);

  this->thumbnailGenerator.setWidth(scaledThumbnailSizePixel);
  this->thumbnailGenerator.setHeight(scaledThumbnailSizePixel);
  QImage thumbnailImage;
  QColor backgroundColor = this->SeriesThumbnail->palette().color(QPalette::Normal, QPalette::Window);
  this->thumbnailGenerator.generateBlankThumbnail(thumbnailImage, backgroundColor);
  QPixmap resultPixmap = QPixmap::fromImage(thumbnailImage);
  QPainter painter;
  if (painter.begin(&resultPixmap))
  {
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    QRect rect = resultPixmap.rect();
    int x = int(rect.width() * 0.5);
    int y = int(rect.height() * 0.5);
    this->drawTextWithShadow(&painter, font, x, y, Qt::AlignCenter, this->Modality);
    painter.end();
  }

  resultPixmap.setDevicePixelRatio(scalingFactor);
  this->SeriesThumbnail->setPixmap(resultPixmap);
  return resultPixmap.toImage();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::drawThumbnail(const QString& dicomFilePath,
                                                    const QString& patientID,
                                                    const QString& studyInstanceUID,
                                                    const QString& seriesInstanceUID,
                                                    const QString& sopInstanceUID,
                                                    const QString& modality,
                                                    int numberOfFrames)
{
  Q_Q(ctkDICOMSeriesItemWidget);

  if (!this->DicomDatabase)
  {
    logger.error("drawThumbnail failed, no DICOM Database has been set. \n");
    return;
  }

  if (patientID.isEmpty() ||
      studyInstanceUID.isEmpty() ||
      seriesInstanceUID.isEmpty() ||
      sopInstanceUID.isEmpty() ||
      modality.isEmpty())
  {
    logger.debug(QString("drawThumbnail failed, missing required parameter(s): "
                         "patientID='%1', studyInstanceUID='%2', seriesInstanceUID='%3', sopInstanceUID='%4', modality='%5'.\n")
                .arg(patientID, studyInstanceUID, seriesInstanceUID, sopInstanceUID, modality));
    return;
  }

  qreal scalingFactor = q->devicePixelRatioF();
  int margin = floor(this->ThumbnailSizePixel / 50.);
  int iconSize = floor(this->ThumbnailSizePixel / 6.);
  int textSize = floor(this->ThumbnailSizePixel / 12.);

  if (this->cachedFontPixelSize != textSize) {
    this->cachedFont = this->SeriesThumbnail->font();
    this->cachedFont.setBold(true);
    this->cachedFont.setPixelSize(textSize);
    this->cachedFontPixelSize = textSize;
  }
  QFont& font = this->cachedFont;

  if (this->ThumbnailImage.isNull())
  {
    QString thumbnailPath = this->DicomDatabase->thumbnailPathForInstance(studyInstanceUID, seriesInstanceUID, sopInstanceUID);
    if (thumbnailPath.isEmpty() && !this->ThumbnailIsGenerating)
    {
      if (dicomFilePath.isEmpty())
      {
        logger.debug("drawThumbnail failed, dicomFilePath is empty. \n");
        return;
      }

      QColor backgroundColor = this->SeriesThumbnail->palette().color(QPalette::Normal, QPalette::Window);
      this->ThumbnailIsGenerating = true;
      this->Scheduler->generateThumbnail(dicomFilePath, patientID, studyInstanceUID, seriesInstanceUID,
                                         sopInstanceUID, modality, backgroundColor,
                                         this->RaiseJobsPriority ? QThread::HighestPriority : QThread::HighPriority);
      return;
    }

    if (thumbnailPath.isEmpty())
    {
      return;
    }
    else
    {
      this->ThumbnailIsGenerating = false;
    }

    if (!this->ThumbnailImage.load(thumbnailPath))
    {
      logger.error("drawThumbnail failed, could not load png file. \n");
      return;
    }
  }

  if (this->cachedPixmapSize != this->ThumbnailSizePixel) {
    this->cachedResultPixmap = QPixmap(this->ThumbnailSizePixel, this->ThumbnailSizePixel);
    this->cachedPixmapSize = this->ThumbnailSizePixel;
  }

  QPixmap& resultPixmap = this->cachedResultPixmap;
  QColor firstPixelColor = this->ThumbnailImage.pixelColor(0, 0);
  resultPixmap.fill(firstPixelColor);

  QPainter painter(&resultPixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
  painter.setFont(font);

  QRectF rect = resultPixmap.rect();
  int x = int((rect.width() * 0.5) - (this->ThumbnailImage.rect().width() * 0.5));
  int y = int((rect.height() * 0.5) - (this->ThumbnailImage.rect().height() * 0.5));

  QPixmap thumbnailPixmap = QPixmap::fromImage(this->ThumbnailImage);
  painter.drawPixmap(x, y, thumbnailPixmap);

  QString topLeftString = ctkDICOMSeriesItemWidget::tr("Series: %1\n%2").arg(this->SeriesNumber).arg(this->Modality);
  if (modality == "SEG")
  {
    topLeftString = ctkDICOMSeriesItemWidget::tr("Series: %1").arg(this->SeriesNumber);
  }
  this->drawTextWithShadow(&painter, font, margin, margin, Qt::AlignTop | Qt::AlignLeft, topLeftString);

  QString rows, columns;
  auto cacheIt = this->instanceRowsColumnsCache.constFind(sopInstanceUID);
  if (cacheIt != this->instanceRowsColumnsCache.constEnd())
  {
    rows = cacheIt.value().first;
    columns = cacheIt.value().second;
  }
  else
  {
    rows = this->DicomDatabase->instanceValue(sopInstanceUID, "0028,0010");
    columns = this->DicomDatabase->instanceValue(sopInstanceUID, "0028,0011");
    this->instanceRowsColumnsCache.insert(sopInstanceUID, qMakePair(rows, columns));
  }
  QString bottomLeftString = rows + "x" + columns + "x" + QString::number(numberOfFrames);
  this->drawTextWithShadow(&painter, font, margin, rect.height() - margin * 2,
                           Qt::AlignBottom | Qt::AlignLeft, bottomLeftString);

  QString iconPath;
  if (this->RetrieveFailed)
  {
    iconPath = ":/Icons/error_red.svg";
  }
  else if (this->IsCloud)
  {
    iconPath = (this->NumberOfDownloads > 0) ? ":/Icons/downloading.svg" : ":/Icons/cloud.svg";
  }
  else if (this->IsVisible)
  {
    iconPath = ":/Icons/visible.svg";
  }
  else if (this->IsLoaded)
  {
    iconPath = ":/Icons/loaded.svg";
  }

  QSharedPointer<QSvgRenderer> renderer;
  if (!iconPath.isEmpty())
  {
    if (!this->svgRenderers.contains(iconPath))
    {
      renderer = QSharedPointer<QSvgRenderer>::create(iconPath);
      this->svgRenderers[iconPath] = renderer;
    }
    else
    {
      renderer = this->svgRenderers[iconPath];
    }
    QPointF topRight = rect.topRight();
    QRectF bounds(topRight.x() - iconSize - margin, topRight.y() + margin, iconSize, iconSize);
    renderer->render(&painter, bounds);
  }

  painter.end();

  resultPixmap.setDevicePixelRatio(scalingFactor);
  this->SeriesThumbnail->setPixmap(resultPixmap);
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
  if (!this->shadowTextLabel)
  {
    this->shadowTextLabel.reset(new QLabel);
    this->shadowEffect.reset(new QGraphicsDropShadowEffect);
    this->shadowEffect->setXOffset(1);
    this->shadowEffect->setYOffset(1);
    this->shadowEffect->setBlurRadius(1);
    this->shadowEffect->setColor(Qt::darkGray);
    this->shadowTextLabel->setGraphicsEffect(this->shadowEffect.data());
    this->shadowTextLabel->setAttribute(Qt::WA_TranslucentBackground);
  }

  if (this->shadowCachedFont != font)
  {
    this->shadowTextLabel->setFont(font);
    this->shadowCachedFont = font;
  }
  if (this->shadowCachedText != text)
  {
    this->shadowTextLabel->setText(text);
    this->shadowCachedText = text;
  }

  QPalette palette = this->shadowTextLabel->palette();
  palette.setColor(QPalette::WindowText, textColor);
  this->shadowTextLabel->setPalette(palette);
  this->shadowTextLabel->adjustSize();
  QPixmap textPixMap = this->shadowTextLabel->grab();
  QRect rect = textPixMap.rect();
  qreal scalingFactor = painter->device()->devicePixelRatioF();
  int textWidth = rect.width() / scalingFactor;
  int textHeight = rect.height() / scalingFactor;

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
  if (!this->DicomDatabase)
  {
    logger.error("Update thumbnail progress bar failed, no DICOM Database has been set. \n");
    return;
  }

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
    this->drawThumbnail(file,
                        this->PatientID,
                        this->StudyInstanceUID,
                        this->SeriesInstanceUID,
                        this->CentralFrameSOPInstanceUID,
                        this->Modality,
                        numberOfFrames);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::resetOperationProgressBar()
{
  this->NumberOfDownloads = 0;
  this->SeriesThumbnail->setOperationProgress(0);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateRetrieveUIOnStarted()
{
  this->ReferenceSeriesInserterJobUID = "";
  this->ReferenceInstanceInserterJobUID = "";
  this->RetrieveFailed = false;
  this->resetOperationProgressBar();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateRetrieveUIOnFailed()
{
  if (!this->DicomDatabase)
  {
    logger.error("Update thumbnail failed, no DICOM Database has been set. \n");
    return;
  }

  this->RetrieveFailed = true;
  this->IsRetrieving = false;

  QString file = this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID);
  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();
  this->drawThumbnail(file,
                      this->PatientID,
                      this->StudyInstanceUID,
                      this->SeriesInstanceUID,
                      this->CentralFrameSOPInstanceUID,
                      this->Modality,
                      numberOfFrames);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidgetPrivate::updateRetrieveUIOnFinished()
{
  if (!this->DicomDatabase)
  {
    logger.error("Update thumbnail failed, no DICOM Database has been set. \n");
    return;
  }

  QStringList instancesList = this->DicomDatabase->instancesForSeries(this->SeriesInstanceUID);
  int numberOfFrames = instancesList.count();

  QStringList filesList = this->DicomDatabase->filesForSeries(this->SeriesInstanceUID);
  filesList.removeAll(QString(""));
  int numberOfFiles = filesList.count();

  if (numberOfFrames > 1 && numberOfFiles == 1)
  {
    // Thumbnail frame has been retrieved, but series has more than 1 frame, continue processing...
    this->IsCloud = true;
    this->SeriesThumbnail->operationProgressBar()->show();
    return;
  }
  else if (numberOfFrames > 0 && numberOfFiles < numberOfFrames)
  {
    // Failed to retrieve all frames, warn the user
    this->RetrieveFailed = true;
    this->IsCloud = false;
    this->SeriesThumbnail->operationProgressBar()->hide();
  }
  else if (numberOfFrames > 0 && numberOfFiles == numberOfFrames)
  {
    // All frames have been retrieved successfully
    this->IsCloud = false;
    this->SeriesThumbnail->operationProgressBar()->hide();
  }
  this->IsRetrieving = false;

  this->drawThumbnail(this->DicomDatabase->fileForInstance(this->CentralFrameSOPInstanceUID),
                      this->PatientID,
                      this->StudyInstanceUID,
                      this->SeriesInstanceUID,
                      this->CentralFrameSOPInstanceUID,
                      this->Modality,
                      numberOfFrames);
  this->SeriesThumbnail->setOperationStatus(ctkThumbnailLabel::Completed);
  this->SeriesThumbnail->setStatusIcon(QIcon(":/Icons/accept.svg"));
}

//----------------------------------------------------------------------------
// ctkDICOMSeriesItemWidget methods

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidget::ctkDICOMSeriesItemWidget(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMSeriesItemWidgetPrivate(*this))
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidget::~ctkDICOMSeriesItemWidget()
{
  Q_D(ctkDICOMSeriesItemWidget);

  QObject::disconnect(d->SeriesThumbnail, SIGNAL(statusPushButtonClicked(bool)),
                      this, SLOT(onOperationStatusButtonClicked(bool)));
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
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, stopJobs, StopJobs);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, bool, setRaiseJobsPriority, RaiseJobsPriority);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, raiseJobsPriority, RaiseJobsPriority);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isCloud, IsCloud);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isLoaded, IsLoaded);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, isVisible, IsVisible);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, bool, setRetrieveFailed, RetrieveFailed);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, bool, retrieveFailed, RetrieveFailed);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, referenceSeriesInserterJobUID, ReferenceSeriesInserterJobUID);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, referenceInstanceInserterJobUID, ReferenceInstanceInserterJobUID);
CTK_SET_CPP(ctkDICOMSeriesItemWidget, int, setThumbnailSizePixel, ThumbnailSizePixel);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, int, thumbnailSizePixel, ThumbnailSizePixel);
CTK_GET_CPP(ctkDICOMSeriesItemWidget, QString, stoppedJobUID, StoppedJobUID);

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
void ctkDICOMSeriesItemWidget::setStopJobs(bool stopJobs)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->StopJobs = stopJobs;
  if (d->StopJobs && d->IsRetrieving)
  {
    d->SeriesThumbnail->setOperationStatus(ctkThumbnailLabel::Failed);
    d->SeriesThumbnail->setStatusIcon(QIcon(":/Icons/error_red.svg"));
    d->updateRetrieveUIOnFailed();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::forceRetrieve()
{
  Q_D(ctkDICOMSeriesItemWidget);

  d->IsCloud = true;
  d->RetrieveFailed = false;
  d->IsRetrieving = true;
  d->StopJobs = false;
  if (d->DicomDatabase)
  {
    d->DicomDatabase->removeSeries(d->SeriesInstanceUID, false, false);
  }
  d->ThumbnailImage = QImage();
  this->generateInstances(true);
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::isRetrieving()
{
  Q_D(ctkDICOMSeriesItemWidget);
  return d->IsRetrieving;
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesItemWidget::isRetrieveFailed()
{
  Q_D(ctkDICOMSeriesItemWidget);
  return d->RetrieveFailed;
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
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMSeriesItemWidget);
  d->Scheduler = scheduler;
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMSeriesItemWidget::dicomDatabase() const
{
  Q_D(const ctkDICOMSeriesItemWidget);
  if (!d->DicomDatabase)
  {
    logger.error("no DICOM Database has been set. \n");
    return nullptr;
  }
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
void ctkDICOMSeriesItemWidget::generateInstances(bool query, bool retrieve)
{
  Q_D(ctkDICOMSeriesItemWidget);
  if (!d->DicomDatabase)
  {
    logger.error("generateInstances failed, no DICOM Database has been set. \n");
    return;
  }

  d->QueryOn = query;
  d->RetrieveOn = retrieve;

  ctkDICOMJobDetail td;
  d->createThumbnail(td);

  QStringList instancesList = d->DicomDatabase->instancesForSeries(d->SeriesInstanceUID);
  if (query && !d->StopJobs &&
      instancesList.count() == 0 &&
      d->Scheduler &&
      d->Scheduler->queryRetrieveServersCount() > 0)
  {
    d->Scheduler->queryInstances(d->PatientID,
                                 d->StudyInstanceUID,
                                 d->SeriesInstanceUID,
                                 d->RaiseJobsPriority ? QThread::HighPriority : QThread::NormalPriority,
                                 d->AllowedServers);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty())
  {
    return;
  }

  if ((td.JobType != ctkDICOMJobResponseSet::JobType::QueryInstances &&
       td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance &&
       td.JobType != ctkDICOMJobResponseSet::JobType::StoreSOPInstance &&
       td.JobType != ctkDICOMJobResponseSet::JobType::ThumbnailGenerator) ||
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
  if (td.JobUID.isEmpty())
  {
    return;
  }

  if ((td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
       td.JobType != ctkDICOMJobResponseSet::JobType::StoreSOPInstance) ||
      td.SeriesInstanceUID != d->SeriesInstanceUID)
  {
    return;
  }

  d->updateThumbnailProgressBar();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobStarted(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (!td.JobUID.isEmpty() &&
      (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      td.SeriesInstanceUID == d->SeriesInstanceUID)
  {
    d->SeriesThumbnail->setOperationStatus(ctkThumbnailLabel::InProgress);
    d->SeriesThumbnail->setStatusIcon(QIcon(":/Icons/pending.svg"));
    d->IsRetrieving = true;
    if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
    {
      d->ReferenceInstanceInserterJobUID = "";
      d->ThumbnailIsGenerating = false;
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->updateRetrieveUIOnStarted();
    }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobUserStopped(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if (!td.JobUID.isEmpty() &&
      (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      td.SeriesInstanceUID == d->SeriesInstanceUID)
  {
    d->SeriesThumbnail->setOperationStatus(ctkThumbnailLabel::Failed);
    d->SeriesThumbnail->setStatusIcon(QIcon(":/Icons/error_red.svg"));

    if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
    {
      d->ReferenceInstanceInserterJobUID = "";
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->ReferenceSeriesInserterJobUID = "";
    }
    d->updateRetrieveUIOnFailed();
    d->StoppedJobUID = td.JobUID;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobFailed(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if (!td.JobUID.isEmpty() &&
      (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      td.SeriesInstanceUID == d->SeriesInstanceUID)
  {
    d->SeriesThumbnail->setOperationStatus(ctkThumbnailLabel::Failed);
    d->SeriesThumbnail->setStatusIcon(QIcon(":/Icons/error_red.svg"));

    if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
    {
      d->ReferenceInstanceInserterJobUID = "";
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->ReferenceSeriesInserterJobUID = "";
    }
    d->updateRetrieveUIOnFailed();
    d->StoppedJobUID = td.JobUID;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onJobFinished(const QVariant &data)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.SeriesInstanceUID == d->SeriesInstanceUID &&
      (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance))
  {
    if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
    {
      if (td.ReferenceInserterJobUID.isEmpty())
      {
        d->ReferenceInstanceInserterJobUID = "StorageListener";
      }
      else
      {
        d->ReferenceInstanceInserterJobUID = td.ReferenceInserterJobUID;
      }
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      if (td.ReferenceInserterJobUID.isEmpty())
      {
        d->ReferenceSeriesInserterJobUID = "StorageListener";
      }
      else
      {
        d->ReferenceSeriesInserterJobUID = td.ReferenceInserterJobUID;
      }
    }
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::Inserter)
  {
    if (d->ReferenceSeriesInserterJobUID == td.JobUID ||
        d->ReferenceSeriesInserterJobUID == "StorageListener")
    {
      d->ReferenceSeriesInserterJobUID = "";
    }
    else if (d->ReferenceInstanceInserterJobUID == td.JobUID ||
             d->ReferenceInstanceInserterJobUID == "StorageListener")
    {
      d->ReferenceInstanceInserterJobUID = "";
    }

    d->updateRetrieveUIOnFinished();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesItemWidget::onOperationStatusButtonClicked(bool)
{
  Q_D(ctkDICOMSeriesItemWidget);

  ctkThumbnailLabel::OperationStatus status = d->SeriesThumbnail->operationStatus();
  if (status == ctkThumbnailLabel::InProgress)
  {
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(),
                                      QStringList(),
                                      QStringList(d->SeriesInstanceUID));
  }
  else if (status > ctkThumbnailLabel::InProgress)
  {
    if (!d->Scheduler->retryJob(d->StoppedJobUID))
    {
    logger.info(QString("Unable to restart job %1 (series level) because the job has been fully cleared from the system. "
                        "Please initiate a new job if further processing is required.").arg(d->StoppedJobUID));
    }
  }
}
