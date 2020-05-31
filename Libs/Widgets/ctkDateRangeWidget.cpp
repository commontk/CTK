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

// Qt includes
#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QDateTimeEdit>

// CTK includes
#include "ctkDateRangeWidget.h"
#include "ui_ctkDateRangeWidget.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkDateRangeWidget");

//-----------------------------------------------------------------------------
class ctkDateRangeWidgetPrivate: public Ui_ctkDateRangeWidget
{
  Q_DECLARE_PUBLIC(ctkDateRangeWidget);
protected:
  ctkDateRangeWidget* const q_ptr;
public:
  ctkDateRangeWidgetPrivate(ctkDateRangeWidget& object);
  /// Automatically select the right radio button based on the date range
  void autoselectRadioButton();

  /// ForceSelectRange is set to true when the user expressively requested
  /// to have "Select Range" option active. This property is set only if
  /// the user clicks on the option or if setSelectRange() is programatically
  /// called
  bool          ForceSelectRange;
  /// DisplayTime is true if the time is displayed in the range widget
  bool          DisplayTime;
};


// --------------------------------------------------------------------------
ctkDateRangeWidgetPrivate::ctkDateRangeWidgetPrivate(ctkDateRangeWidget& object)
  :q_ptr(&object)
{
  this->ForceSelectRange = false;
  this->DisplayTime = true;
}
// -------------------------------------------------------------------------
void ctkDateRangeWidgetPrivate::autoselectRadioButton()
{
  Q_Q(ctkDateRangeWidget);
  QDate startDate = q->startDateTime().date();
  QDate endDate = q->endDateTime().date();
  #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
  QDateTime startOfDay = q->startDateTime().date().startOfDay();
  QDateTime endOfDay = q->startDateTime().date().endOfDay();
  #else
  QDateTime startOfDay = QDateTime(q->startDateTime().date());
  QDateTime endOfDay = QDateTime(q->endDateTime().date());
  #endif
  if (this->ForceSelectRange)
    {
    this->SelectRangeRadioButton->setChecked(true);
    }
  else if (q->isAnyDate())
    {
    this->AnyDateRadioButton->setChecked(true);
    }
  else if (q->startDateTime() != startOfDay ||
           q->endDateTime() != endOfDay)
    {
    this->SelectRangeRadioButton->setChecked(true);
    }
  else if (startDate.addDays(1) == endDate &&
           startDate == QDate::currentDate())
    {
    this->TodayRadioButton->setChecked(true);
    }
  else if (startDate.addDays(1) == endDate &&
           endDate == QDate::currentDate())
    {
    this->YesterdayRadioButton->setChecked(true);
    }
  else if (startDate.addDays(7) == endDate &&
           endDate == QDate::currentDate())
    {
    this->LastWeekRadioButton->setChecked(true);
    }
  else if (startDate.addDays(31) == endDate &&
           endDate == QDate::currentDate())
    {
    this->LastMonthRadioButton->setChecked(true);
    }
  else
    {
    this->SelectRangeRadioButton->setChecked(true);
    }
}

// --------------------------------------------------------------------------
ctkDateRangeWidget::ctkDateRangeWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkDateRangeWidgetPrivate(*this))
{
  Q_D(ctkDateRangeWidget);

  d->setupUi(this);

  d->DateRangeWidget->setVisible(d->SelectRangeRadioButton->isChecked());

  this->setDisplayTime(false);
  this->setDateTimeRange(QDateTime(), QDateTime());

  // Note that we connect on the clicked() signal and not the toggled.
  // The clicked() signal is fired only when the USER clicks the radio button
  // and not when the button is checked programatically (except using click()).
  QObject::connect(d->AnyDateRadioButton, SIGNAL(clicked()),
                   this, SLOT(setAnyDate()));
  QObject::connect(d->TodayRadioButton, SIGNAL(clicked()),
                   this, SLOT(setToday()));
  QObject::connect(d->YesterdayRadioButton, SIGNAL(clicked()),
                   this, SLOT(setYesterday()));
  QObject::connect(d->LastWeekRadioButton, SIGNAL(clicked()),
                   this, SLOT(setLastWeek()));
  QObject::connect(d->LastMonthRadioButton, SIGNAL(clicked()),
                   this, SLOT(setLastMonth()));
  QObject::connect(d->SelectRangeRadioButton, SIGNAL(clicked()),
                   this, SLOT(setSelectRange()));

  QObject::connect(d->StartDate, SIGNAL(dateTimeChanged(QDateTime)),
                   this, SIGNAL(startDateTimeChanged(QDateTime)));
  QObject::connect(d->EndDate, SIGNAL(dateTimeChanged(QDateTime)),
                   this, SIGNAL(endDateTimeChanged(QDateTime)));
  QObject::connect(d->StartDate, SIGNAL(dateTimeChanged(QDateTime)),
                   this, SLOT(onDateTimeChanged()));
  QObject::connect(d->EndDate, SIGNAL(dateTimeChanged(QDateTime)),
                   this, SLOT(onDateTimeChanged()));
}

// --------------------------------------------------------------------------
ctkDateRangeWidget::~ctkDateRangeWidget()
{
}

// --------------------------------------------------------------------------
QDateTime ctkDateRangeWidget::startDateTime()const
{
  Q_D(const ctkDateRangeWidget);
  return d->StartDate->dateTime();
}

// --------------------------------------------------------------------------
QDateTime ctkDateRangeWidget::endDateTime()const
{
  Q_D(const ctkDateRangeWidget);
  return d->EndDate->dateTime();
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setStartDateTime(QDateTime dateTime)
{
  Q_D(ctkDateRangeWidget);
  d->StartDate->setDateTime(dateTime);
  d->autoselectRadioButton();
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setEndDateTime(QDateTime dateTime)
{
  Q_D(ctkDateRangeWidget);
  d->EndDate->setDateTime(dateTime);
  d->autoselectRadioButton();
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setDateTimeRange(QDateTime startDateTime, QDateTime endDateTime)
{
  Q_D(ctkDateRangeWidget);
  d->StartDate->setDateTime(startDateTime.isValid() ?
    startDateTime : d->StartDate->minimumDateTime());
  d->EndDate->setDateTime(endDateTime.isValid() ?
    endDateTime : d->EndDate->maximumDateTime());
  d->autoselectRadioButton();
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setDateRange(QDate startDate, QDate endDate)
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
  this->setDateTimeRange(startDate.startOfDay(), endDate.endOfDay());
  #else
  this->setDateTimeRange(QDateTime(startDate), QDateTime(endDate));
  #endif
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setAnyDate()
{
  Q_D(ctkDateRangeWidget);
  d->ForceSelectRange = false;
  this->setDateTimeRange(QDateTime(), QDateTime());
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setToday()
{
  Q_D(ctkDateRangeWidget);
  d->ForceSelectRange = false;
  QDate today = QDate::currentDate();
  this->setDateRange(today, today.addDays(1));
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setYesterday()
{
  Q_D(ctkDateRangeWidget);
  d->ForceSelectRange = false;
  QDate today = QDate::currentDate();
  this->setDateRange(today.addDays(-1), today);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setLastWeek()
{
  Q_D(ctkDateRangeWidget);
  d->ForceSelectRange = false;
  QDate today = QDate::currentDate();
  this->setDateRange(today.addDays(-7), today);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setLastMonth()
{
  Q_D(ctkDateRangeWidget);
  d->ForceSelectRange = false;
  QDate today = QDate::currentDate();
  this->setDateRange(today.addMonths(-1), today);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setSelectRange()
{
  Q_D(ctkDateRangeWidget);
  d->SelectRangeRadioButton->setChecked(true);
  d->ForceSelectRange = true;
}

// -------------------------------------------------------------------------
bool ctkDateRangeWidget::isAnyDate()const
{
  Q_D(const ctkDateRangeWidget);
  return this->startDateTime() == d->StartDate->minimumDateTime() &&
         this->endDateTime() == d->EndDate->maximumDateTime();
}

// -------------------------------------------------------------------------
void ctkDateRangeWidget::setDisplayTime(bool displayTime)
{
  Q_D(ctkDateRangeWidget);
  d->DisplayTime = displayTime;
  if ( displayTime )
    {
    d->StartDate->setDisplayFormat( QString( "MMM dd, yyyy HH:mm:ss") );
    d->EndDate->setDisplayFormat( QString( "MMM dd, yyyy HH:mm:ss") );
    }
  else
    {
    d->StartDate->setDisplayFormat( QString( "MMM dd, yyyy") );
    d->EndDate->setDisplayFormat( QString( "MMM dd, yyyy") );
    }
}

// -------------------------------------------------------------------------
bool ctkDateRangeWidget::displayTime()const
{
  logger.error("including time in the date range is not supported now");
  Q_D(const ctkDateRangeWidget);
  return d->DisplayTime;
}

// -------------------------------------------------------------------------
void ctkDateRangeWidget::onDateTimeChanged()
{
  Q_D(ctkDateRangeWidget);
  d->autoselectRadioButton();
}
