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
#include <QTabWidget>
#include <QBoxLayout>

// CTK includes
#include "ctkWorkflowTabWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowTabWidget");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowTabWidgetPrivate: public ctkPrivate<ctkWorkflowTabWidget>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowTabWidget);
  ctkWorkflowTabWidgetPrivate();

  QTabWidget* clientArea;
};

// --------------------------------------------------------------------------
// ctkWorkflowTabWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowTabWidgetPrivate::ctkWorkflowTabWidgetPrivate()
{
  this->clientArea = 0;
}

// --------------------------------------------------------------------------
// ctkWorkflowTabWidget methods

// --------------------------------------------------------------------------
ctkWorkflowTabWidget::ctkWorkflowTabWidget(QWidget* newParent) : Superclass(newParent)
{
  CTK_INIT_PRIVATE(ctkWorkflowTabWidget);
}

// --------------------------------------------------------------------------
QTabWidget* ctkWorkflowTabWidget::clientArea()
{
  CTK_D(ctkWorkflowTabWidget);

  if (!d->clientArea)
    {
    d->clientArea = new QTabWidget;

    if (!this->clientSection()->layout())
      {
      this->clientSection()->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
      }
    this->clientSection()->layout()->addWidget(d->clientArea);
    }
  return d->clientArea;
}

// --------------------------------------------------------------------------
void ctkWorkflowTabWidget::addWidgetToClientArea(QWidget* widget, const QString& label)
{
  if (widget)
    {
    QTabWidget* clientArea = this->clientArea();

    clientArea->addTab(widget, label);
    }
}

// --------------------------------------------------------------------------
QWidget* ctkWorkflowTabWidget::getWidgetFromIndex(int index)
{
  CTK_D(ctkWorkflowTabWidget);

  if (!d->clientArea)
    {
    logger.error("getWidgetFromIndex - clientArea is Null");
    return 0;;
    }

  return d->clientArea->widget(index);
}

// --------------------------------------------------------------------------
void ctkWorkflowTabWidget::setCurrentWidget(QWidget* widget)
{
  CTK_D(ctkWorkflowTabWidget);

  if (!d->clientArea)
    {
    logger.error("setCurrentWidget - clientArea is Null");
    return;
    }

  if (widget)
    {
    d->clientArea->setCurrentWidget(widget);
    }
}
