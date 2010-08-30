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
#include <QBoxLayout>
#include <QWidget>
#include <QMap>

// CTK includes
#include "ctkWorkflowAbstractPagedWidget.h"
#include "ctkWorkflowWidgetStep.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkWorkflowAbstractPagedWidgetPrivate: public ctkPrivate<ctkWorkflowAbstractPagedWidget>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowAbstractPagedWidget);
  ctkWorkflowAbstractPagedWidgetPrivate(){}

  // maintain a map of <widget, pageArea> pairs to find the
  // page that a widget belongs to
  typedef QMap<QWidget*, QWidget*> widgetToPagedWidgetMapType;
  typedef QMap<QWidget*, QWidget*>::iterator widgetToPagedWidgetMapIterator;
  widgetToPagedWidgetMapType widgetToPagedWidgetMap;
};

// --------------------------------------------------------------------------
// ctkWorkflowAbstractPagedWidgetMethods

// --------------------------------------------------------------------------
ctkWorkflowAbstractPagedWidget::ctkWorkflowAbstractPagedWidget(QWidget* newParent) :
    Superclass(newParent)
{
  CTK_INIT_PRIVATE(ctkWorkflowAbstractPagedWidget);
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::setClientAreaDirection(const QBoxLayout::Direction& direction, int index)
{
  CTK_D(ctkWorkflowAbstractPagedWidget);
  // change the direction for all pages if not given an index
  if (index == -1)
    {
    ctkWorkflowAbstractPagedWidgetPrivate::widgetToPagedWidgetMapIterator it = d->widgetToPagedWidgetMap.begin();
    ctkWorkflowAbstractPagedWidgetPrivate::widgetToPagedWidgetMapIterator end = d->widgetToPagedWidgetMap.end();
    for (; it != end; ++it)
      {
      this->changeWidgetDirection(it.value(), direction);
      }
    }
  // change the direction for a specific index
  else
    {
    QWidget* pageArea = this->getWidgetFromIndex(index);
    if (pageArea)
      {
      this->changeWidgetDirection(pageArea, direction);
      }
    }

  this->setPrivateClientAreaDirection(direction);
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addStepArea(ctkWorkflowWidgetStep* step, int index)
{
  this->addStepArea(step, index, "");
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addStepArea(ctkWorkflowWidgetStep* step, const QString& label)
{
  this->addStepArea(step, -1, label);
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addStepArea(ctkWorkflowWidgetStep* step, int index, const QString& label)
{
  if (step)
    {
    this->addWidget(step->stepArea(), index, label);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addWidget(QWidget* widget)
{
  this->addWidget(widget, -1, "");
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addWidget(QWidget* widget, int index)
{
  this->addWidget(widget, index, "");
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addWidget(QWidget* widget, const QString& label)
{
  this->addWidget(widget, -1, label);
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::addWidget(QWidget* widget, int index, const QString& label)
{
  if (widget)
    {
    CTK_D(ctkWorkflowAbstractPagedWidget);
    QWidget* pageArea;
    // adding a new page
    if (index == -1)
      {
      pageArea = new QWidget();
      pageArea->setLayout(new QBoxLayout(this->clientAreaDirection()));
      }
    // if adding to a previously created page, try to get the parent
    // widget for the page with the specified index
    else
      {
      pageArea = this->getWidgetFromIndex(index);
      }

    // add the widget to the page
    if (pageArea)
      {
      pageArea->layout()->addWidget(widget);
      d->widgetToPagedWidgetMap[widget] = pageArea;

      // if we are creating a new page, then add it to the client area
      if (index == -1)
        {
        this->addWidgetToClientArea(pageArea, label);
        }
      }
    // if an invalid page index was given, create a new page
    else
      {
      this->addWidget(widget, -1, label);
      }
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::showWidget(QWidget* widget)
{
  if (widget)
    {
    CTK_D(ctkWorkflowAbstractPagedWidget);

    // try to get the page widget for the page corresponding to the
    // given widget
    QWidget* pageArea = d->widgetToPagedWidgetMap[widget];
    if (!pageArea)
      {
      std::cerr << "cannot show widget without corresponding page" << std::endl;
      return;
      }

    this->setCurrentWidget(pageArea);
    widget->show();
    }
}
