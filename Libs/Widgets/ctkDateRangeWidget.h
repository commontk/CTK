/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDateRangeWidget_h
#define __ctkDateRangeWidget_h

// Qt includes
#include <QDateTimeEdit>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkDateRangeWidgetPrivate;
class QDateTime;

/// \ingroup Widgets
///
/// ctkDateRangeWidget allows the user to select a time range between two
/// dates
class CTK_WIDGETS_EXPORT ctkDateRangeWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY startDateTimeChanged)
  Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY endDateTimeChanged)
  Q_PROPERTY(bool displayTime READ displayTime WRITE setDisplayTime)

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  /// If \li parent is null, ctkDateRangeWidget will be a top-level widget
  /// \note The \li parent can be set later using QWidget::setParent()
  /// By default, the range is "Any Date"
  explicit ctkDateRangeWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkDateRangeWidget();

  ///
  /// This property holds whether the date range includes time
  /// If includeTime is disabled (the default), the widget only shows dates
  /// If includeTime is enabled the date widgets display time as well as date
  void setDisplayTime(bool includeTime);
  bool displayTime()const;

  /// Access the start date/times
  /// The returned date is never NULL/empty, but set to
  /// QDateTimeEdit::minimumDateTime
  QDateTime startDateTime() const;
  
  /// Access the start date/times
  /// The returned date is never NULL/empty, but set to 
  /// QDateTimeEdit::maximumDateTime
  QDateTime endDateTime() const;
  
  /// Utility function that returns true if the range correspond to any date
  /// It can be useful if the time must be handled specially in that case.
  /// Returns true if any of the start or end date is invalid.
  bool isAnyDate()const;

public Q_SLOTS:
  /// Set the start date.
  /// If the date is null or invalid, it will be automatically converted into
  /// a valid date (14 September 1752)
  void setStartDateTime(QDateTime start);
  /// Set the end date.
  /// If the date is null or invalid, it will be automatically converted into
  /// a valid date (31 December, 7999 and a time of 23:59:59 and 999 milliseconds)
  void setEndDateTime(QDateTime end);
  ///
  /// Utility function that set the start and end values at once
  void setDateTimeRange(QDateTime start, QDateTime end);
  void setDateRange(QDate start, QDate end);

  ///
  /// handle clicks on radio buttons
  void setAnyDate();
  void setToday();
  void setYesterday();
  void setLastWeek();
  void setLastMonth();
  void setSelectRange();

Q_SIGNALS:
  /// Fired when the start date is changed
  void startDateTimeChanged(const QDateTime& value);
  /// Fired when the end date is changed
  void endDateTimeChanged(const QDateTime& value);

protected Q_SLOTS:
  void onDateTimeChanged();

protected:
  QScopedPointer<ctkDateRangeWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDateRangeWidget);
  Q_DISABLE_COPY(ctkDateRangeWidget);

};

#endif
