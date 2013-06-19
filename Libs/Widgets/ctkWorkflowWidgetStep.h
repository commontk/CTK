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

#ifndef __ctkWorkflowWidgetStep_h
#define __ctkWorkflowWidgetStep_h

// Qt includes
#include <QWidget>
#include <QBoxLayout>
#include <QFlags>
#include <QIcon>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"

class ctkWorkflowGroupBox;

class ctkWorkflowWidgetStepPrivate;

/// \ingroup Widgets
///
/// \brief ctkWorkflowWidgetStep is a convienience class to quickly
/// construct a ctkWorkflowStep with a user interface.
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

class CTK_WIDGETS_EXPORT ctkWorkflowWidgetStep : public QWidget, public ctkWorkflowStep
{
  Q_OBJECT
  Q_PROPERTY(QString stepid READ id WRITE setId)
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
  Q_PROPERTY(QString statusText READ statusText)
  Q_PROPERTY(QString backButtonText READ backButtonText WRITE setBackButtonText)
  Q_PROPERTY(QString nextButtonText READ nextButtonText WRITE setNextButtonText)
  Q_FLAGS(ButtonBoxHint ButtonBoxHints)
  Q_ENUMS(ButtonBoxHint)
  Q_PROPERTY(ButtonBoxHints buttonBoxHints READ buttonBoxHints WRITE setButtonBoxHints)
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

  explicit ctkWorkflowWidgetStep(QWidget* newParent = 0);
  explicit ctkWorkflowWidgetStep(const QString& newId, QWidget* newParent = 0);
  virtual ~ctkWorkflowWidgetStep();

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

  /// Returns the QWidget onto which this step's user interface elements are placed.
  virtual QWidget* stepArea();

  /// Set/get whether a showUserInterfaceCommand has been provided in
  /// a separate QObject (see method 2 described for
  /// showUserInterface())
  virtual bool hasShowUserInterfaceCommand()const;
  virtual void setHasShowUserInterfaceCommand(bool flag);

  /// Set/get whether a createUserInterfaceCommand has been provided in
  /// a separate QObject (see method 2 described for
  /// createUserInterface())
  virtual bool hasCreateUserInterfaceCommand()const;
  virtual void setHasCreateUserInterfaceCommand(bool flag);

protected:

  /// Creates the user interface associated with this step.
  virtual void createUserInterface(){}

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

private:

  //Q_DECLARE_PRIVATE(ctkWorkflowWidgetStep);
  // Since this class derives from both QWidget and ctkWorkflowStep,
  // let's specify which 'd_ptr' to use to avoid ambiguous reference
  inline ctkWorkflowWidgetStepPrivate* d_func() { return reinterpret_cast<ctkWorkflowWidgetStepPrivate *>(qGetPtrHelper(ctkWorkflowStep::d_ptr)); }
  inline const ctkWorkflowWidgetStepPrivate* d_func() const { return reinterpret_cast<const ctkWorkflowWidgetStepPrivate *>(qGetPtrHelper(ctkWorkflowStep::d_ptr)); }
  friend class ctkWorkflowWidgetStepPrivate;

  Q_DISABLE_COPY(ctkWorkflowWidgetStep);
  friend class ctkWorkflowGroupBox; // For access to showUserInterface()
};

 Q_DECLARE_OPERATORS_FOR_FLAGS(ctkWorkflowWidgetStep::ButtonBoxHints)

#endif

