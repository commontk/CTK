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

#ifndef __ctkWorkflowWidgetStep_h
#define __ctkWorkflowWidgetStep_h 

// QT includes
class QObject;
class QWidget;
#include <QBoxLayout>
#include <QFlags>

// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"
class ctkWorkflowWidget;
class ctkWorkflowButtonBoxWidget;

class ctkWorkflowWidgetStepPrivate;

///
/// \brief ctkWorkflowWidgetStep is a convienience class to quickly
/// construct a ctkWorkflowStep with a user interface, for use in a ctkWorkflowWidget.
///
/// It embeds a QWidget* stepArea, onto which step-specific widgets can be placed.
/// The showUserInterface() and hideUserInterface() commands of ctkWorkflowStep are
/// written for you, and, if you desire, the step's "Next" and/or "Back" buttons are
/// added with the appropriate signals and slots.
/// To create a custom step, you can derive from this class and
/// implement only two functions:
/// 1) ctkWorkflowWidgetStep::populateStepWidgetsList(), to define the
/// step-specific widgets;
/// 2) ctkWorkflowWidgetStep::validate(const QString&), to validate the processing
/// state associated with this step.
/// For additional customization, you can reimplement
/// showUserInterface() and hideUserInterface() in derived classes.
/// \sa showUserInterface() hideUserInterface()

class CTK_WIDGETS_EXPORT ctkWorkflowWidgetStep : public ctkWorkflowStep
{ 
  Q_OBJECT
  Q_FLAGS(ButtonBoxHint)

public:

  enum ButtonBoxHint {
    NoHints = 0x0,
    BackButtonHidden = 0x1,
    BackButtonDisabled = 0x2,
    NextButtonHidden = 0x4,
    NextButtonDisabled = 0x8,
    ButtonBoxHidden = 0x10
  };
  Q_DECLARE_FLAGS(ButtonBoxHints, ButtonBoxHint)

  typedef ctkWorkflowStep Superclass;
  explicit ctkWorkflowWidgetStep(ctkWorkflow* newWorkflow, const QString& newId);
  virtual ~ctkWorkflowWidgetStep(){}

  virtual QString backButtonText()const;
  virtual void setBackButtonText(const QString& name);

  virtual QString nextButtonText()const;
  virtual void setNextButtonText(const QString& name);

  // /// TODO - same text for all finish buttons
  // virtual QList<QString> finishButtonTexts()const;
  // virtual void setFinishButtonText(const QString& name);  
  // virtual void setFinishButtonTexts(const QList<QString>);

  ///
  /// Set/get whether a populateStepWidgetsListCommand has been
  /// provided in a separate QObject (see method 2 described for
  /// populateStepWidgetsList())
  virtual int hasPopulateStepWidgetsListCommand()const;
  virtual void setHasPopulateStepWidgetsListCommand(int flag);

  /// Set/get whether a showUserInterfaceCommand has been provided in
  /// a separate QObject (see method 2 described for
  /// showUserInterface())
  virtual int hasShowUserInterfaceCommand()const;
  virtual void setHasShowUserInterfaceCommand(int flag);

  ///
  /// Sets the direction of the QBoxLayout that manages this widget's
  /// client area, i.e. the layout manager for the area onto which the
  /// step-specific widgets should be placed. (default
  /// QBoxLayout::TopToBottom)
  QBoxLayout::Direction stepAreaDirection()const;
  void setStepAreaDirection(const QBoxLayout::Direction& direction);

  /// Provided for advanced customization: get the stepArea, which is
  /// the parent widget for all of this step's step-specific widgets
  /// and which will be placed onto the workflow widget's client area.
  /// If you do not associate a layout manager with the step area, the
  /// default layout manager is a QBoxLayout with direction
  /// QBoxLayout::TopToBottom.  Use the function
  /// setStepAreaDirection() to change the direction, or for
  /// additional customization use
  /// ctkWorkflowWidgetStep::stepArea()->setLayout()
  virtual QWidget* stepArea();

  /// Set/get whether or not to associate a buttonBoxWidget with this step (default false)
  bool hasButtonBoxWidget()const;
  void setHasButtonBoxWidget(bool newHasButtonBoxWidget);

  /// The widget with the 'next', 'back' and 'goTo' buttons
  virtual ctkWorkflowButtonBoxWidget* buttonBoxWidget();

  void setButtonBoxHints(ButtonBoxHints buttonBoxHints);
  ButtonBoxHints buttonBoxHints()const;

protected:

  virtual void addButtons();

protected slots:

  ///
  /// For each custom step using the given implementations of
  /// showUserInterface() and hideUserInterface(), you must do the
  /// following within a reimplmentation of this function:
  /// 1) Create instances of the step's widget(s)
  /// 2) Set any desired attributes of the step's widget(s)
  /// 3) Add the widget(s) to the stepWidgetsList given in the
  /// parameter
  /// These widgets will be added to the stepArea *in the order in
  /// which they were added to the list*, according to the step area's
  /// layout
  /// Developers can either reimplement the populateStepWidgetsList()
  /// method in a subclass of ctkWorkflowWidgetStep (*do* call the
  /// superclass's populateStepWidgetsList() method first), or create
  /// a ctkWorkflowWidgetStep instance and set the
  /// populateStepWidgetListCommand to point to a callback of their
  /// choice.
  /// Reimplemented in derived classes.
  virtual void populateStepWidgetsList(QList<QWidget*>& stepWidgetsList);

  ///
  /// Shows the user interface associated with this step.
  /// Do not call showUserInterface() directly, use onEntry() instead.
  /// An implementation of showUserInterface is provided; to use it
  /// you must provide an implementation of
  /// populateStepWidgetsList().
  /// For additional customization, developers can either reimplement
  /// the showUserInterface() method in a subclass of
  /// ctkWorkflowWidgetStep (*do* call the superclass's
  /// showUserInterface() method first), or create a
  /// ctkWorkflowWidgetStep instance and set the
  /// showUserInterfaceCommand to point to a callback of their choice.
  /// Reimplement in derived classes.
  virtual void showUserInterface();

  ///
  /// TODO
  virtual void evaluatePopulateStepWidgetsListResults();

signals:

  void invokePopulateStepWidgetsListCommand(QList<QWidget*>& stepWidgetsList)const;

  void populateStepWidgetsListComplete()const;

  void invokeShowUserInterfaceCommand()const;

  void showUserInterfaceComplete()const;

private:
  CTK_DECLARE_PRIVATE(ctkWorkflowWidgetStep);
  friend class ctkWorkflowWidget; // For access to ..

  ///
  /// Sets the widget's layout's direction, if widget's layout is a
  /// QBoxLayout.  Used internally by setWorkflowWidgetDirection() and
  /// setClientAreaDirection()
  virtual void changeWidgetDirection(QWidget* widget, const QBoxLayout::Direction& direction);
};

 Q_DECLARE_OPERATORS_FOR_FLAGS(ctkWorkflowWidgetStep::ButtonBoxHints)

#endif

