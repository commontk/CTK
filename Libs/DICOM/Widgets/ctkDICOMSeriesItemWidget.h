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

#ifndef __ctkDICOMSeriesItemWidget_h
#define __ctkDICOMSeriesItemWidget_h

// Qt includes
#include <QVariant>
#include <QWidget>

// ctkDICOMCore includes
#include "ctkDICOMJobResponseSet.h"
class ctkDICOMDatabase;
class ctkDICOMScheduler;

// ctkDICOMWidgets includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMSeriesItemWidgetPrivate;
class ctkDICOMStudyItemWidget;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMSeriesItemWidget : public QWidget
{
  Q_OBJECT;

  Q_PROPERTY(QStringList allowedServers READ allowedServers WRITE setAllowedServers NOTIFY allowedServersChanged);

  Q_PROPERTY(QString seriesItem READ seriesItem WRITE setSeriesItem NOTIFY seriesItemChanged);
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID NOTIFY patientIDChanged);
  Q_PROPERTY(QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID NOTIFY studyInstanceUIDChanged);
  Q_PROPERTY(QString seriesInstanceUID READ seriesInstanceUID WRITE setSeriesInstanceUID NOTIFY seriesInstanceUIDChanged);
  Q_PROPERTY(QString seriesNumber READ seriesNumber WRITE setSeriesNumber NOTIFY seriesNumberChanged);
  Q_PROPERTY(QString modality READ modality WRITE setModality NOTIFY modalityChanged);
  Q_PROPERTY(bool stopJobs READ stopJobs WRITE setStopJobs NOTIFY stopJobsChanged);
  Q_PROPERTY(bool raiseJobsPriority READ raiseJobsPriority WRITE setRaiseJobsPriority NOTIFY raiseJobsPriorityChanged);
  Q_PROPERTY(bool retrieveFailed READ retrieveFailed WRITE setRetrieveFailed NOTIFY retrieveFailedChanged);
  Q_PROPERTY(int thumbnailSizePixel READ thumbnailSizePixel WRITE setThumbnailSizePixel NOTIFY thumbnailSizePixelChanged);

  Q_PROPERTY(QString seriesDescription READ seriesDescription WRITE setSeriesDescription NOTIFY seriesDescriptionChanged);

  Q_PROPERTY(bool isCloud READ isCloud CONSTANT);
  Q_PROPERTY(QString referenceSeriesInserterJobUID READ referenceSeriesInserterJobUID CONSTANT);
  Q_PROPERTY(QString referenceInstanceInserterJobUID READ referenceInstanceInserterJobUID CONSTANT);
  Q_PROPERTY(QString stoppedJobUID READ stoppedJobUID CONSTANT);

public:
  typedef QWidget Superclass;
  explicit ctkDICOMSeriesItemWidget(QWidget* parent = nullptr);
  virtual ~ctkDICOMSeriesItemWidget();

  ///@{
  /// Series Item
  void setSeriesItem(const QString& seriesItem);
  QString seriesItem() const;
  ///@}

  ///@{
  /// Patient ID
  void setPatientID(const QString& patientID);
  QString patientID() const;
  ///@}

  ///@{
  /// Study instance UID
  void setStudyInstanceUID(const QString& studyInstanceUID);
  QString studyInstanceUID() const;
  ///@}

  ///@{
  /// Series instance UID
  void setSeriesInstanceUID(const QString& seriesInstanceUID);
  QString seriesInstanceUID() const;
  ///@}

  ///@{
  /// Series Number
  void setSeriesNumber(const QString& seriesNumber);
  QString seriesNumber() const;
  ///@}

  ///@{
  /// Modality
  void setModality(const QString& modality);
  QString modality() const;
  ///@}

  ///@{
  /// Series Description
  void setSeriesDescription(const QString& seriesDescription);
  QString seriesDescription() const;
  ///@}

  ///@{
  /// Stop Series widget to run new jobs
  void setStopJobs(bool stopJobs);
  bool stopJobs() const;
  ///@}

  ///@{
  /// Set high priority to all jobs run from the Series widget
  void setRaiseJobsPriority(bool raiseJobsPriority);
  bool raiseJobsPriority() const;
  ///@}

  /// Series lives in the server
  bool isCloud() const;

  /// Force retrieve for series. If the series was already fetched,
  /// it will be retrieved again.
  void forceRetrieve();

  ///@{
  /// in case the retrieve job failed
  void setRetrieveFailed(bool retrieveFailed);
  bool retrieveFailed() const;
  ///@}

  ///@{
  /// Return the referenceInserterJobUID
  QString referenceSeriesInserterJobUID() const;
  QString referenceInstanceInserterJobUID() const;
  ///@}

  /// Series has been loaded by the parent widget
  bool isLoaded() const;

  /// Series is visible in the parent widget
  bool isVisible() const;

  ///@{
  /// Set the thumbnail size in pixel
  /// 200 by default
  void setThumbnailSizePixel(int thumbnailSizePixel);
  int thumbnailSizePixel() const;
  ///@}

  ///@{
  /// Allowed Servers
  /// Empty by default
  void setAllowedServers(const QStringList& allowedServers);
  QStringList allowedServers() const;
  ///@}

  /// Return the scheduler.
  Q_INVOKABLE ctkDICOMScheduler* scheduler() const;
  /// Return the scheduler as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<ctkDICOMScheduler> schedulerShared() const;
  /// Set the scheduler.
  Q_INVOKABLE void setScheduler(ctkDICOMScheduler& scheduler);
  /// Set the scheduler as a shared pointer
  /// (not Python-wrappable).
  void setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler);

  /// Return the Dicom Database.
  Q_INVOKABLE ctkDICOMDatabase* dicomDatabase() const;
  /// Return Dicom Database as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<ctkDICOMDatabase> dicomDatabaseShared() const;
  /// Set the Dicom Database.
  Q_INVOKABLE void setDicomDatabase(ctkDICOMDatabase& dicomDatabase);
  /// Set the Dicom Database as a shared pointer
  /// (not Python-wrappable).
  void setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);

  /// Last stopped job information operated by this widget
  Q_INVOKABLE QString stoppedJobUID() const;

Q_SIGNALS:
  void seriesItemChanged(const QString &);
  void patientIDChanged(const QString &);
  void studyInstanceUIDChanged(const QString &);
  void seriesInstanceUIDChanged(const QString &);
  void seriesNumberChanged(const QString &);
  void modalityChanged(const QString &);
  void seriesDescriptionChanged(const QString &);
  void retrieveFailedChanged(bool);
  void thumbnailSizePixelChanged(int);
  void stopJobsChanged(bool);
  void raiseJobsPriorityChanged(bool);
  void allowedServersChanged(const QStringList &);

public Q_SLOTS:
  void generateInstances(bool query = true, bool retrieve = true);
  void updateGUIFromScheduler(const QVariant&);
  void updateSeriesProgressBar(const QVariant&);
  void onJobStarted(const QVariant&);
  void onJobUserStopped(const QVariant&);
  void onJobFailed(const QVariant&);
  void onJobFinished(const QVariant&);
  void onOperationStatusButtonClicked(bool);

protected:
  QScopedPointer<ctkDICOMSeriesItemWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesItemWidget);
  Q_DISABLE_COPY(ctkDICOMSeriesItemWidget);
};

#endif
