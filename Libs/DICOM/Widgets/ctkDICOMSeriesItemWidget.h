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
class ctkDICOMDatabase;
class ctkDICOMScheduler;

// ctkDICOMWidgets includes
#include "ctkDICOMWidgetsExport.h"
class ctkDICOMSeriesItemWidgetPrivate;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMSeriesItemWidget : public QWidget
{
  Q_OBJECT;
  Q_PROPERTY(QString seriesItem READ seriesItem WRITE setSeriesItem);
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID);
  Q_PROPERTY(QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID);
  Q_PROPERTY(QString seriesInstanceUID READ seriesInstanceUID WRITE setSeriesInstanceUID);
  Q_PROPERTY(QString seriesNumber READ seriesNumber WRITE setSeriesNumber);
  Q_PROPERTY(QString modality READ modality WRITE setModality);
  Q_PROPERTY(QString seriesDescription READ seriesDescription WRITE setSeriesDescription);
  Q_PROPERTY(bool isCloud READ isCloud);
  Q_PROPERTY(bool retrieveFailed READ retrieveFailed WRITE setRetrieveFailed);
  Q_PROPERTY(int thumbnailSizePixel READ thumbnailSizePixel WRITE setThumbnailSizePixel);
  Q_PROPERTY(bool stopJobs READ stopJobs WRITE setStopJobs);
  Q_PROPERTY(bool raiseJobsPriority READ raiseJobsPriority WRITE setRaiseJobsPriority);

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
  void setStopJobs(const bool& stopJobs);
  bool stopJobs() const;
  ///@}

  ///@{
  /// Set high priority to all jobs run from the Series widget
  void setRaiseJobsPriority(const bool& raiseJobsPriority);
  bool raiseJobsPriority() const;
  ///@}

  /// Series lives in the server
  bool isCloud() const;

  ///@{
  /// in case the retrieve job failed
  void setRetrieveFailed(const bool& retrieveFailed);
  bool retrieveFailed() const;
  ///@}

  /// Series has been loaded by the parent widget
  bool IsLoaded() const;

  /// Series is visible in the parent widget
  bool IsVisible() const;

  ///@{
  /// Set the thumbnail size in pixel
  /// 200 by default
  void setThumbnailSizePixel(int thumbnailSizePixel);
  int thumbnailSizePixel() const;
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

public Q_SLOTS:
  void generateInstances();
  void updateGUIFromScheduler(QVariant data);
  void updateSeriesProgressBar(QVariant data);

protected:
  QScopedPointer<ctkDICOMSeriesItemWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesItemWidget);
  Q_DISABLE_COPY(ctkDICOMSeriesItemWidget);
};

#endif
