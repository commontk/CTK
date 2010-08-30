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
#include <QStackedWidget>
#include <QBoxLayout>

// CTK includes
#include "ctkWorkflowStackedWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowStackedWidget");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowStackedWidgetPrivate: public ctkPrivate<ctkWorkflowStackedWidget>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowStackedWidget);
  ctkWorkflowStackedWidgetPrivate();

  QStackedWidget* ClientArea;
};

// --------------------------------------------------------------------------
// ctkWorkflowStackedWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowStackedWidgetPrivate::ctkWorkflowStackedWidgetPrivate()
{
  this->ClientArea = 0;
}

// --------------------------------------------------------------------------
// ctkWorkflowStackedWidget methods

// --------------------------------------------------------------------------
ctkWorkflowStackedWidget::ctkWorkflowStackedWidget(QWidget* newParent) : Superclass(newParent)
{
  CTK_INIT_PRIVATE(ctkWorkflowStackedWidget);
}

// --------------------------------------------------------------------------
QStackedWidget* ctkWorkflowStackedWidget::clientArea()
{
  CTK_D(ctkWorkflowStackedWidget);

  if (!d->ClientArea)
    {
    d->ClientArea = new QStackedWidget;

    if (!this->clientSection()->layout())
      {
      this->clientSection()->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
      }
    this->clientSection()->layout()->addWidget(d->ClientArea);
    }
  return d->ClientArea;
}

// --------------------------------------------------------------------------
void ctkWorkflowStackedWidget::addWidgetToClientArea(QWidget* widget, const QString& label)
{
  Q_UNUSED(label);

  if (!widget)
    {
    return;
    }

  // set the default layout if there isn't one
  if (!this->clientArea()->layout())
    {
    this->clientArea()->setLayout(new QBoxLayout(this->clientAreaDirection()));
    return;
    }

  this->clientArea()->layout()->addWidget(widget);
}

// --------------------------------------------------------------------------
QWidget* ctkWorkflowStackedWidget::getWidgetFromIndex(int index)
{
  CTK_D(ctkWorkflowStackedWidget);

  if (!d->ClientArea)
    {
    logger.error("getWidgetFromIndex - ClientArea is Null");
    return 0;
    }

  return d->ClientArea->widget(index);
}

// --------------------------------------------------------------------------
void ctkWorkflowStackedWidget::setCurrentWidget(QWidget* widget)
{
  CTK_D(ctkWorkflowStackedWidget);

  if (!d->ClientArea)
    {
    logger.error("setCurrentWidget - ClientArea is Null");
    return;
    }

  if (widget)
    {
    d->ClientArea->setCurrentWidget(widget);
    }
}
