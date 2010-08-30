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

#ifndef __ctkWorkflowAbstractPagedWidget_h
#define __ctkWorkflowAbstractPagedWidget_h

// Qt includes

// CTK includes
#include "ctkWorkflowWidget.h"
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
class ctkWorkflowWidgetStep;

class ctkWorkflowAbstractPagedWidgetPrivate;


/// ctkWorkflowAbstractPagedWidget is the basis for a workflow with a
/// user interface containing multiple pages.
/// It is an abstract child of ctkWorkflowWidget and therefore is a
/// ctkWorkflow (i.e. a state machine).  It has a client area
/// consisting of multiple pages (i.e. a QStackedWidget or a
/// QTabWidget), onto which step-specific widgets can be placed.
/// Multiple workflow steps can correspond to the same page.  The main
/// widget can be inserted directly inside another user interface.

class CTK_WIDGETS_EXPORT ctkWorkflowAbstractPagedWidget : public ctkWorkflowWidget
{
  Q_OBJECT

public:

  typedef ctkWorkflowWidget Superclass;
  explicit ctkWorkflowAbstractPagedWidget(QWidget* newParent = 0);
  virtual ~ctkWorkflowAbstractPagedWidget(){}

  /// only need to do if you're specifying an index/label, otherwise
  /// the workflow's steps will be added automatically
  virtual void addStepArea(ctkWorkflowWidgetStep* step, const QString& label);
  virtual void addStepArea(ctkWorkflowWidgetStep* step, int index);
  virtual void addStepArea(ctkWorkflowWidgetStep* step, int index, const QString& label);

  /// Add a widget to the client area, which is the area where
  /// custom-step content should be placed.  If you are adding
  /// ctkWorkflowWidgetSteps to this workflow, you will probably want
  /// to use ctkWorkflowWidget::addNextStep() or
  /// ctkWorkflowWidget::addStep() instead.
  /// A workflow containing a user interface is made up of steps
  /// (ctkWorkflowWidgetStep).  Each step should either:
  /// 1) reimplement ctkWorkflowWidgetStep::populateStepWidgetsList or
  /// set its populateStepWidgetsListCommand to point to a method that
  /// will add the custom-step's widgets to the list; or
  /// 2) reimplement ctkWorkflowWidgetStep::showUserInterface or set
  /// its showUserInterfaceCommand to point to a method that will
  /// display the step's user interface.  Within that method, each of
  /// the custom step's widgets should be added to the workflow
  /// widget's clientArea using the addWidget() function.
  /// If not given an index: creates a new page on the client area.
  /// If given an index: adds a widget to a previously created page on
  /// the client area, as denoted by the page's index.  If the given
  /// index does not correspond to a previously created page, then a
  /// new page will be created.
  /// The optional label may or may not be considered by the concrete
  /// implementation of this class.
  using ctkWorkflowWidget::addWidget;
  virtual void addWidget(QWidget* widget);
  virtual void addWidget(QWidget* widget, const QString& label);
  virtual void addWidget(QWidget* widget, int index);
  virtual void addWidget(QWidget* widget, int index, const QString& label);

  /// Sets the direction of the QBoxLayout that manages this widget's
  /// client area, i.e. the layout manager for the area onto which the
  /// step's widgets should be placed (default
  /// QBoxLayout::TopToBottom)
  /// To change the layout for a specific page, provide its index.
  /// Calling ctkPagedWorkflowWidget::setClientAreaDirect() without an
  /// index changes the layout direction for all previously created
  /// and future pages.
  virtual void setClientAreaDirection(const QBoxLayout::Direction& direction, int index = -1);

  /// Provided for advanced customization: get the client area.
  /// A workflow containing a user interface is made up of steps
  /// (ctkWorkflowWidgetStep).  Each step should either:
  /// 1) reimplement ctkWorkflowWidgetStep::populateStepWidgetsList or
  /// set its populateStepWidgetsListCommand to point to a method that
  /// will add the custom-step's widgets to the list; or
  /// 2) reimplement ctkWorkflowWidgetStep::showUserInterface or set
  /// its showUserInterfaceCommand to point to a method that will
  /// display the step's user interface.  Within that method, each of
  /// the custom step's widgets should be added to the workflow
  /// widget's clientArea using the addWidget() function.
  /// If you do not associate a layout manager with the clientArea,
  /// the default layout manager is a QBoxLayout with direction
  /// QBoxLayout::TopToBottom.  Use the function
  /// setClientAreaDirection() to change the direction.
  /// For additional customization of layouts, note that
  /// ctkWorkflowAbstractPagedWidget()->clientArea()->setLayout() will
  /// likely not allow you to customize the client area layout.
  /// Customize specific pages instead using
  /// ctkWorkflowAbstractPagedWidget::getWidgetFromIndex(index)->setLayout()
  virtual QWidget* clientArea() = 0;

  /// Provided for advanced customization: get the widget
  /// corresponding to the page of the given index
  virtual QWidget* getWidgetFromIndex(int index) = 0;

  /// Provided for advanced customization: shows a given widget within
  /// the client area.
  /// A workflow containing a user interface is made up of steps
  /// (ctkWorkflowWidgetStep).  Each step should either:
  /// 1) reimplement ctkWorkflowWidgetStep::populateStepWidgetsList or
  /// set its populateStepWidgetsListCommand to point to a method that
  /// will add the custom-step's widgets to the list; or
  /// 2) reimplement ctkWorkflowWidgetStep::showUserInterface or set
  /// its showUserInterfaceCommand to point to a method that will
  /// display the step's user interface.  Within that method, each of
  /// the custom step's widgets should be added to the workflow
  /// widget's clientArea using the addWidget() function.
  /// Using showWidget() rather than widget()->show() ensures correct
  /// display of the widget within the client area.  The widget
  /// parameter should correspond to a widget that was previously used
  /// with the addWidget() function.
  /// Reimplement in derived classes.
  virtual void showWidget(QWidget* widget);

protected:

  /// Add a widget to a new page of the client area, with a page label
  /// that may or may not be considered by the derived class
  virtual void addWidgetToClientArea(QWidget* widget, const QString& label) = 0;

  /// Set the current widget to that of the page containing the given widget
  virtual void setCurrentWidget(QWidget* widget) = 0;

private:
  CTK_DECLARE_PRIVATE(ctkWorkflowAbstractPagedWidget);

};

#endif
