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

#ifndef __ctkDICOMJobListWidget_h
#define __ctkDICOMJobListWidget_h

#include "ctkDICOMWidgetsExport.h"

// Qt includes
#include <QWidget>
#include <QVariant>

class ctkDICOMJobListWidgetPrivate;
class ctkDICOMScheduler;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMJobListWidget : public QWidget
{
  Q_OBJECT;

public:
  typedef QWidget Superclass;
  explicit ctkDICOMJobListWidget(QWidget* parent = nullptr);
  virtual ~ctkDICOMJobListWidget();

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

public Q_SLOTS:
  void onJobInitialized(QVariant);
  void onJobQueued(QVariant);
  void onJobStarted(QList<QVariant>);
  void onJobAttemptFailed(QList<QVariant>);
  void onJobFailed(QList<QVariant>);
  void onJobUserStopped(QList<QVariant>);
  void onJobFinished(QList<QVariant>);
  void onProgressJobDetail(QList<QVariant>);

  void onFilterTextChanged(QString);
  void onFilterColumnChanged(QString);

  void onJobsViewSelectionChanged();
  void onSelectAllButtonClicked();
  void onStopButtonClicked();
  void onRetryButtonClicked();
  void onResetFiltersButtonClicked();
  void onShowCompletedButtonToggled(bool);
  void onClearCompletedButtonClicked();
  void onClearAllButtonClicked();

Q_SIGNALS:
  /// Emitted when a job row is selected
  void patientSelected(const QString&, const QString&, const QString&);

protected:
  QScopedPointer<ctkDICOMJobListWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMJobListWidget);
  Q_DISABLE_COPY(ctkDICOMJobListWidget);
};

#endif
