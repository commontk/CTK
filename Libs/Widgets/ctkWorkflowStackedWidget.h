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

#ifndef __ctkWorkflowStackedWidget_h
#define __ctkWorkflowStackedWidget_h

// Qt includes
#include <QStackedWidget>

// CTK includes
#include "ctkWorkflowAbstractPagedWidget.h"
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"

class ctkWorkflowStackedWidgetPrivate;

///
/// ctkWorkflowStackedWidget is the basis for a workflow with a user
/// interface containing a QStackedWidget.
/// It is a child of ctkWorkflowAbstractPagedWidget and therefore is a
/// ctkWorkflow (i.e. a state machine).  Its client area is a
/// QStackedWidget, onto which step-specific widgets can be placed.
/// Multiple workflow steps can correspond to the same page of the
/// QStackedWidget.  The main widget can be inserted directly inside
/// another user interface.

class CTK_WIDGETS_EXPORT ctkWorkflowStackedWidget : public ctkWorkflowAbstractPagedWidget
{
  Q_OBJECT
    // TODO DESIGNER: properties here

public:
  ///
  /// Superclass typedef
  typedef ctkWorkflowAbstractPagedWidget Superclass;

  ///
  /// Constructors
  explicit ctkWorkflowStackedWidget(QWidget* parent = 0);
  virtual ~ctkWorkflowStackedWidget();

  ///
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
  /// ctkWorkflowStackedWidget()->clientArea()->setLayout() will likely
  /// not allow you to customize the client area layout.  Customize
  /// specific pages instead using
  /// ctkWorkflowStackedWidget::getWidgetFromIndex(index)->setLayout()
  virtual QStackedWidget* clientArea();

  ///
  /// Provided for advanced customization: get the widget
  /// corresponding to the page of the given index
  virtual QWidget* getWidgetFromIndex(int index);

protected:
  ///
  /// Add a widget to a new page of the client area.
  /// The label is not considered by this class.
  virtual void addWidgetToClientArea(QWidget* widget, const QString& label);

  ///
  /// Set the current widget to that of the page containing the given
  /// widget
  virtual void setCurrentWidget(QWidget* widget) ;

private:
  CTK_DECLARE_PRIVATE(ctkWorkflowStackedWidget);

};

#endif
