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
#include <QWidget>
#include <QString>
#include <QDebug>

// CTK includes
#include "ctkWorkflowTabWidget.h"
#include "ctkWorkflowButtonBoxWidget.h"
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

  QTabWidget* ClientArea;
};

// --------------------------------------------------------------------------
// ctkWorkflowTabWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowTabWidgetPrivate::ctkWorkflowTabWidgetPrivate()
{
}

// --------------------------------------------------------------------------
// ctkWorkflowTabWidget methods

// --------------------------------------------------------------------------
ctkWorkflowTabWidget::ctkWorkflowTabWidget(QWidget* newParent) : Superclass(newParent)
{
  CTK_INIT_PRIVATE(ctkWorkflowTabWidget);
  CTK_D(ctkWorkflowTabWidget);
  d->ClientArea = 0;
}

// --------------------------------------------------------------------------
QWidget* ctkWorkflowTabWidget::clientArea()
{
  CTK_D(ctkWorkflowTabWidget);
  return d->ClientArea;
}

// --------------------------------------------------------------------------
void ctkWorkflowTabWidget::initClientArea()
{
  CTK_D(ctkWorkflowTabWidget);
  if (!d->ClientArea)
    {
    d->ClientArea = new QTabWidget(this);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowTabWidget::createNewPage(QWidget* widget)
{
  CTK_D(ctkWorkflowTabWidget);
  Q_ASSERT(d->ClientArea);
  if (widget)
    {
    d->ClientArea->addTab(widget, "");
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowTabWidget::showPage(QWidget* widget, const QString& label)
{
  CTK_D(ctkWorkflowTabWidget);
  Q_ASSERT(d->ClientArea);
  if (widget)
    {
    d->ClientArea->setCurrentWidget(widget);
    int index = d->ClientArea->indexOf(widget);
    d->ClientArea->setTabText(index, label);
    }
}

