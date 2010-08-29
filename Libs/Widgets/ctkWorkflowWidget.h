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

#ifndef __ctkWorkflowWidget_h
#define __ctkWorkflowWidget_h

// Qt includes
#include <QWidget>
class QPushButton;
#include <QBoxLayout>

// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
class ctkWorkflow;
class ctkWorkflowStep;
class ctkWorkflowWidgetStep;
class ctkWorkflowButtonBoxWidget;

class ctkWorkflowWidgetPrivate;

/// \brief ctkWorkflowWidget is the basis for a workflow with a user interface.
///
/// It is a ctkWorkflow (i.e. a state machine) that has a QWidget* mainWidget.
/// The mainWidget allows for a widget title, sub-title, pre-text, post-text and
/// error text to be displayed, as well as a QWidget* client area, onto which
/// step-specific widgets can be placed.  Multiple workflow steps can correspond to
/// the same client area.
/// The main widget can be inserted directly inside another user interface.

class CTK_WIDGETS_EXPORT ctkWorkflowWidget : public QWidget
{
  Q_OBJECT

public:

  typedef QWidget Superclass;
  explicit ctkWorkflowWidget(QWidget* parent = 0);
  virtual ~ctkWorkflowWidget();

  ///
  ///
  virtual ctkWorkflow* workflow()const;
  virtual void setWorkflow(ctkWorkflow* workflow);

  /// \brief Add a widget to the client area, which is the area where
  /// custom-step content should be placed.
  ///
  /// If you are adding ctkWorkflowWidgetSteps to this workflow, you will probably want
  /// to use ctkWorkflowWidget::addNextStep() or ctkWorkflowWidget::addStep() instead.
  /// A workflow containing a user interface is made up of steps
  /// (ctkWorkflowWidgetStep).
  ///
  /// Each step should either:
  /// 1) reimplement ctkWorkflowWidgetStep::populateStepWidgetsList or
  /// set its populateStepWidgetsListCommand to point to a method that
  /// will add the custom-step's widgets to the list; or
  /// 2) reimplement ctkWorkflowWidgetStep::showUserInterface or set
  /// its showUserInterfaceCommand to point to a method that will
  /// display the step's user interface.  Within that method, each of
  /// the custom step's widgets should be added to the workflow
  /// widget's clientArea using the addWidget() function.
  virtual void addWidget(QWidget* widget);

  ///
  /// If hideWidgetsOfNonCurrentSteps is turned on, then a step's
  /// widgets will be hidden when that step is not the current step.
  /// If it is turned off, then they will be shown but disabled.
  /// (Default OFF).
  virtual int hideWidgetsOfNonCurrentSteps()const;
  virtual void setHideWidgetsOfNonCurrentSteps(int flag);

  ///
  /// Set/get the title text (usually a few words), located in the top
  /// area.
  /// Note that this method is called automatically to display the

  /// \brief Set/get the title text (usually a few words), located in the top area.
  /// \note This method is called automatically to display the  
  /// name of the ctkWorkflow's currentStep() step (see the
  /// ctkWorkflowStep::name() method).
  virtual QString title()const;
  virtual void setTitle(const QString& title);

  /// \brief Set/get the subtitle text (usually a short sentence or two),
  /// located in the top area below the title.
  /// \note This method is called automatically to display the
  /// description of the ctkWorkflow's currentStep() step.
  /// \sa ctkWorkflowStep::description()
  virtual QString subTitle()const;
  virtual void setSubTitle(const QString& subTitle);

  /// \brief Set/get the pre-text, i.e. the contents of a convenience text
  /// section placed just above the client area.
  virtual QString preText()const;
  virtual void setPreText(const QString& preText);

  /// \brief Set/get the post-text, i.e. the contents of a convenience text
  /// section placed just below the client area.
  virtual QString postText()const;
  virtual void setPostText(const QString& postText);

  /// \brief Set/get the error text, i.e. the contents of the convenience
  /// section placed below the client area.
  ///
  /// This is typically used by a step's ctkWorkflowStep::validate(const QString&) function to report an
  /// error when validation fails
  virtual QString errorText()const;
  virtual void setErrorText(const QString& errorText);

  /// Sets the direction of the QBoxLayout that manages this widget's
  /// client area, i.e. the layout manager for the area onto which the
  /// step's widgets should be placed (default QBoxLayout::TopToBottom)
  QBoxLayout::Direction clientAreaDirection()const;
  void setClientAreaDirection(const QBoxLayout::Direction& direction);

  /// Sets the direction for the QBoxLayout that manages this widget.
  /// This is the layout of the entire widget, i.e. the layout manager
  /// that places the title, subtitle, preText, client, postText and
  /// errorText sections. (default QBoxLayout::TopToBottom)
  QBoxLayout::Direction direction()const;
  void setDirection(const QBoxLayout::Direction& direction);

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
  /// setClientAreaDirection() to change the direction, or for
  /// additional customization use
  /// ctkWorkflowWidget::clientArea()->setLayout()
  virtual QWidget* clientArea();

  /// Set/get whether or not to associate a buttonBoxWidget with this step (default true)
  bool hasButtonBoxWidget()const;
  void setHasButtonBoxWidget(bool newHasButtonBoxWidget);

  /// The widget with the 'next', 'back' and 'goTo' buttons
  virtual ctkWorkflowButtonBoxWidget* buttonBoxWidget()const;

public slots:
  ///
  ///
  virtual void updateClientArea(ctkWorkflowStep* currentStep);

protected:
  /// Sets the widget's layout's direction, if widget's layout is a
  /// QBoxLayout.
  /// Used internally by setWorkflowWidgetDirection() and setClientAreaDirection()
  virtual void changeWidgetDirection(QWidget* widget, const QBoxLayout::Direction& direction);


  /// These functions are provided to access ctkWorkflowWidgetPrivate's
  /// attributes from ctkWorkflowWidget's descendents.
  QWidget* clientSection()const;
  void setPrivateClientAreaDirection(const QBoxLayout::Direction& direction);

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
  virtual void showWidget(QWidget* widget);

private:
  CTK_DECLARE_PRIVATE(ctkWorkflowWidget);

};

#endif

