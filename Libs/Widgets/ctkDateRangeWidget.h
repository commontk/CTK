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

#ifndef __ctkDateRangeWidget_h
#define __ctkDateRangeWidget_h

// Qt includes
#include <QDateTimeEdit>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkDateRangeWidgetPrivate;
class QDateTime;

///
/// ctkDateRangeWidget is a wrapper around a ctkDoubleRangeSlider and 2 QSpinBoxes
/// \image html http://www.commontk.org/images/1/14/CtkDateRangeWidget.png
/// \sa ctkSliderSpinBoxWidget, ctkDoubleRangeSlider, QSpinBox
class CTK_WIDGETS_EXPORT ctkDateRangeWidget : public QWidget
{
  Q_OBJECT
    // placeholder - not yet used
  Q_PROPERTY(bool includeTime READ includeTime WRITE setIncludeTime)

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  /// If \li parent is null, ctkDateRangeWidget will be a top-leve widget
  /// \note The \li parent can be set later using QWidget::setParent()
  explicit ctkDateRangeWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkDateRangeWidget();

  ///
  /// This property holds whether the date range includes time
  /// If tracking is disabled (the default), the widget only shows dates
  /// If includeTime is enabled the date widgets display time as well as date
  void setIncludeTime(bool includeTime);
  bool includeTime()const;

  ///
  /// Access the start and end date/times
  QDateTime startDateTime() const;
  QDateTime endDateTime() const;

public slots:
  ///
  /// Reset the slider and spinbox to zero (value and position)
  void setStartDateTime(QDateTime start);
  void setEndDateTime(QDateTime end);
  ///
  /// Utility function that set the start and end values at once
  void setDateTimeRange(QDateTime start, QDateTime end);

  ///
  /// handle clicks on radio buttons
  void onAnyDate();
  void onToday();
  void onYesterday();
  void onLastWeek();
  void onLastMonth();
  void onSelectRange();

signals:
  /// 
  /// signals
  void startDateTimeChanged(QDateTime value);
  void endDateTimeChanged(QDateTime value);

protected slots:
  ///
  /// None

protected:
  ///
  /// None

protected:
  QScopedPointer<ctkDateRangeWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDateRangeWidget);
  Q_DISABLE_COPY(ctkDateRangeWidget);

};

#endif
