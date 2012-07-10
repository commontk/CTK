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

=========================================================================*/

// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

// STD includes
#include <iostream>

// Qt includes
#include <QTimer>

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMQueryWidget");


//----------------------------------------------------------------------------
class ctkDICOMQueryWidgetPrivate: public Ui_ctkDICOMQueryWidget
{
public:
  ctkDICOMQueryWidgetPrivate();
  ~ctkDICOMQueryWidgetPrivate();

  QTimer* SearchTimer;
  int SearchIdleTime;
};

//----------------------------------------------------------------------------
// ctkDICOMQueryWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidgetPrivate::ctkDICOMQueryWidgetPrivate(){
  // set default of search idle time to 500 ms
  this->SearchIdleTime = 500;

  // Initialize search timer
  this->SearchTimer = new QTimer();
  this->SearchTimer->setSingleShot(true);
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidgetPrivate::~ctkDICOMQueryWidgetPrivate(){
  this->SearchTimer->deleteLater();
}

//----------------------------------------------------------------------------
// ctkDICOMQueryWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::ctkDICOMQueryWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMQueryWidgetPrivate)
{
  Q_D(ctkDICOMQueryWidget);
  
  d->setupUi(this);

  d->NameSearch->setFocus(Qt::PopupFocusReason);

  connect(d->NameSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  connect(d->StudySearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  connect(d->SeriesSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  connect(d->IdSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  connect(d->DateRangeWidget, SIGNAL(endDateTimeChanged(QDateTime)), this, SLOT(startTimer()));
  connect(d->DateRangeWidget, SIGNAL(startDateTimeChanged(QDateTime)), this, SLOT(startTimer()));
  connect(d->ModalityWidget, SIGNAL(selectedModalitiesChanged(QStringList)), this, SLOT(startTimer()));

  connect(d->SearchTimer, SIGNAL(timeout()), this, SIGNAL(parameterChanged()));

  connect(d->NameSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  connect(d->StudySearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  connect(d->SeriesSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  connect(d->IdSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
  Q_D(ctkDICOMQueryWidget);

  disconnect(d->NameSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  disconnect(d->StudySearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  disconnect(d->SeriesSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  disconnect(d->IdSearch, SIGNAL(textChanged(QString)), this, SLOT(startTimer()));
  disconnect(d->DateRangeWidget, SIGNAL(endDateTimeChanged(QDateTime)), this, SLOT(startTimer()));
  disconnect(d->DateRangeWidget, SIGNAL(startDateTimeChanged(QDateTime)), this, SLOT(startTimer()));
  disconnect(d->ModalityWidget, SIGNAL(selectedModalitiesChanged(QStringList)), this, SLOT(startTimer()));

  disconnect(d->SearchTimer, SIGNAL(timeout()), this, SIGNAL(parameterChanged()));

  disconnect(d->NameSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  disconnect(d->StudySearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  disconnect(d->SeriesSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
  disconnect(d->IdSearch, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}


//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQueryWidget::parameters()
{
  Q_D(ctkDICOMQueryWidget);

  QMap<QString,QVariant> parameters;

  parameters["Name"] = d->NameSearch->text();
  parameters["Study"] = d->StudySearch->text();
  parameters["Series"] = d->SeriesSearch->text();
  parameters["ID"] = d->IdSearch->text();

  if ( !d->ModalityWidget->areAllModalitiesSelected() )
  { // some PACS (conquest) don't seem to accept list of modalities,
    // so don't include the list at all when all modalities are desired
    // TODO: think about how to fix this for conquest at the query level
    parameters["Modalities"] = d->ModalityWidget->selectedModalities();
  }

  if ( !d->DateRangeWidget->isAnyDate() )
  {
    QDate startDate = d->DateRangeWidget->startDateTime().date();
    QDate endDate = d->DateRangeWidget->endDateTime().date();
    parameters["StartDate"] = startDate.toString("yyyyMMdd");
    parameters["EndDate"] = endDate.toString("yyyyMMdd");
  }

  return parameters;
}

//----------------------------------------------------------------------------
void ctkDICOMQueryWidget::startTimer()
{
  Q_D(ctkDICOMQueryWidget);

  d->SearchTimer->stop();
  d->SearchTimer->start(d->SearchIdleTime);
}

//----------------------------------------------------------------------------
void ctkDICOMQueryWidget::onReturnPressed()
{
  emit returnPressed();
}
