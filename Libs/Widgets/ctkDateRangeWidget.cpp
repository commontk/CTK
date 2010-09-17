/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

  bool          includeTime;
};


// --------------------------------------------------------------------------
ctkDateRangeWidgetPrivate::ctkDateRangeWidgetPrivate(ctkDateRangeWidget& object)
  :q_ptr(&object)
{
  this->includeTime = false;
}

// --------------------------------------------------------------------------
ctkDateRangeWidget::ctkDateRangeWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkDateRangeWidgetPrivate(*this))
{
  Q_D(ctkDateRangeWidget);
  
  d->setupUi(this);

  QObject::connect(d->AnyDate, SIGNAL(clicked()),this, SLOT(onAnyDate()));
  QObject::connect(d->Today, SIGNAL(clicked()),this, SLOT(onToday()));
  QObject::connect(d->Yesterday, SIGNAL(clicked()),this, SLOT(onYesterday()));
  QObject::connect(d->LastWeek, SIGNAL(clicked()),this, SLOT(onLastWeek()));
  QObject::connect(d->LastMonth, SIGNAL(clicked()),this, SLOT(onLastMonth()));
  //QObject::connect(d->SelectRange, SIGNAL(clicked()),this, SLOT(onSelectRange()));
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
  Q_D(const ctkDateRangeWidget);
  return d->StartDate->setDateTime(dateTime);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setEndDateTime(QDateTime dateTime)
{
  Q_D(const ctkDateRangeWidget);
  return d->EndDate->setDateTime(dateTime);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::setDateTimeRange(QDateTime startDateTime, QDateTime endDateTime)
{
  Q_D(const ctkDateRangeWidget);
  return d->StartDate->setDateTime(startDateTime);
  return d->EndDate->setDateTime(endDateTime);
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onAnyDate()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected AnyDate");
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onToday()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected Today");
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onYesterday()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected Yesterday");
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onLastWeek()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected LastWeek");
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onLastMonth()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected LastMonth");
}

// --------------------------------------------------------------------------
void ctkDateRangeWidget::onSelectRange()
{
  Q_D(const ctkDateRangeWidget);
  logger.info("selected SelectRange");
}


// -------------------------------------------------------------------------
void ctkDateRangeWidget::setIncludeTime(bool includeTime)
{
  Q_D(ctkDateRangeWidget);
  d->includeTime = includeTime;
  if ( includeTime )
  {
    // TODO: set time format 
    d->StartDate->setDisplayFormat( QString( "MMM dd, yyyy") );
    d->EndDate->setDisplayFormat( QString( "MMM dd, yyyy") );
  } 
  else 
  {
    d->StartDate->setDisplayFormat( QString( "MMM dd, yyyy") );
    d->EndDate->setDisplayFormat( QString( "MMM dd, yyyy") );
  }
}

// -------------------------------------------------------------------------
bool ctkDateRangeWidget::includeTime()const
{
  Q_D(const ctkDateRangeWidget);
  return d->includeTime;
}

