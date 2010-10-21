/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkWorkflowAbstractWidgetStep_h
#define __ctkWorkflowAbstractWidgetStep_h 

// QT includes
class QObject;
class QWidget;
class QIcon;
#include <QBoxLayout>
#include <QFlags>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"
//class ctkWorkflowButtonBoxWidget;
class ctkWorkflowGroupBox;

class ctkWorkflowAbstractWidgetStepPrivate;

///
/// \brief ctkWorkflowAbstractWidgetStep is a convienience class to quickly
/// construct a ctkWorkflowStep with a user interface.
///
/// It embeds a QWidget* stepArea, onto which step-specific widgets can be placed.
/// The showUserInterface() and hideUserInterface() commands of ctkWorkflowStep are
/// written for you, and, if you desire, the step's "Next" and/or "Back" buttons are
/// added with the appropriate signals and slots.
/// To create a custom step, you can derive from this class and
/// implement only two functions:
/// 1) ctkWorkflowAbstractWidgetStep::populateStepWidgetsList(), to define the
/// step-specific widgets;
/// 2) ctkWorkflowAbstractWidgetStep::validate(const QString&), to validate the processing
/// state associated with this step.
/// For additional customization, you can reimplement
/// showUserInterface() and hideUserInterface() in derived classes.
/// \sa showUserInterface() hideUserInterface()

class CTK_WIDGETS_EXPORT ctkWorkflowAbstractWidgetStep : public ctkWorkflowStep
{
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
  explicit ctkWorkflowAbstractWidgetStep(ctkWorkflow* newWorkflow, const QString& newId);
  virtual ~ctkWorkflowAbstractWidgetStep();

  /// \brief Override the back button text of any ctkWorkflowButtonBox when this step
  /// is the current step
  virtual QString backButtonText()const;
  virtual void setBackButtonText(const QString& name);

  /// \brief Override the next button text of any ctkWorkflowButtonBox when this step
  /// is the current step
  virtual QString nextButtonText()const;
  virtual void setNextButtonText(const QString& name);

  /// \brief Override the button visibility of any ctkWorkflowButtonBox when this step is the
  /// current step
  void setButtonBoxHints(ButtonBoxHints buttonBoxHints);
  ButtonBoxHints buttonBoxHints()const;

  /// \brief Associate an icon with this step (ex. used by ctkWorkflowButtonBox to display an icon
  /// on 'goTo' buttons).
  QIcon icon()const;
  void setIcon(const QIcon& newIcon);

  /// Get the QObject associated with this step, to connect signals/slots
  QObject* ctkWorkflowAbstractWidgetStepQObject();

  /// Returns the QWidget onto which this step's user interface elements are placed.
  virtual QWidget* stepArea() = 0;

  /// Set/get whether a showUserInterfaceCommand has been provided in
  /// a separate QObject (see method 2 described for
  /// showUserInterface())
  virtual int hasShowUserInterfaceCommand()const;
  virtual void setHasShowUserInterfaceCommand(int flag);

  /// Set/get whether a createUserInterfaceCommand has been provided in
  /// a separate QObject (see method 2 described for
  /// createUserInterface())
  virtual int hasCreateUserInterfaceCommand()const;
  virtual void setHasCreateUserInterfaceCommand(int flag);

protected:

  /// Creates the user interface associated with this step.
  virtual void createUserInterface() = 0;

  /// Prepares the step to be shown.
  virtual void showUserInterface();

  /// \brief Signal (emitted by the private implementation) indicating that the step's
  /// createUserInterface() method should be called.
  /// \sa createUserInterface()
  void invokeCreateUserInterfaceCommand()const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's
  /// createUserInterface() method has completed.
  /// \sa createUserInterface()
  void createUserInterfaceComplete()const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's
  /// 'showUserInterface() method should be called.
  /// \sa showUserInterface()
  void invokeShowUserInterfaceCommand()const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's
  /// showUserInterface() method has completed.
  /// \sa showUserInterface()
  void showUserInterfaceComplete()const;

protected:
  QScopedPointer<ctkWorkflowAbstractWidgetStepPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowAbstractWidgetStep);
  Q_DISABLE_COPY(ctkWorkflowAbstractWidgetStep);
  friend class ctkWorkflowGroupBox; // For access to showUserInterface()
};

 Q_DECLARE_OPERATORS_FOR_FLAGS(ctkWorkflowAbstractWidgetStep::ButtonBoxHints)

#endif

