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

// Qt includes
#include <QStackedWidget>
#include <QWidget>
#include <QString>

// CTK includes
#include "ctkWorkflowStackedWidget.h"
#include "ctkWorkflowButtonBoxWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkWorkflowStackedWidgetPrivate
{
public:
  ctkWorkflowStackedWidgetPrivate();

  QStackedWidget* ClientArea;
};

// --------------------------------------------------------------------------
// ctkWorkflowStackedWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowStackedWidgetPrivate::ctkWorkflowStackedWidgetPrivate()
{
}

// --------------------------------------------------------------------------
// ctkWorkflowStackedWidget methods

// --------------------------------------------------------------------------
ctkWorkflowStackedWidget::ctkWorkflowStackedWidget(QWidget* newParent) : Superclass(newParent)
  , d_ptr(new ctkWorkflowStackedWidgetPrivate)
{
  Q_D(ctkWorkflowStackedWidget);
  d->ClientArea = 0;
}

// --------------------------------------------------------------------------
ctkWorkflowStackedWidget::~ctkWorkflowStackedWidget()
{
}

// --------------------------------------------------------------------------
QWidget* ctkWorkflowStackedWidget::clientArea()
{
  Q_D(ctkWorkflowStackedWidget);
  return d->ClientArea;
}

// --------------------------------------------------------------------------
void ctkWorkflowStackedWidget::initClientArea()
{
  Q_D(ctkWorkflowStackedWidget);
  if (!d->ClientArea)
    {
    d->ClientArea = new QStackedWidget(this);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowStackedWidget::createNewPage(QWidget* widget)
{  
  Q_D(ctkWorkflowStackedWidget);
  Q_ASSERT(d->ClientArea);

  if (widget)
    {
    d->ClientArea->layout()->addWidget(widget);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowStackedWidget::showPage(QWidget* widget, const QString& label)
{
  Q_UNUSED(label);

  Q_D(ctkWorkflowStackedWidget);
  Q_ASSERT(d->ClientArea);

  if (widget)
    {
    d->ClientArea->setCurrentWidget(widget);
    }
}
