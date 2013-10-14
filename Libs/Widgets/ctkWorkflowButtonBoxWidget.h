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

#ifndef __ctkWorkflowButtonBoxWidget_h
#define __ctkWorkflowButtonBoxWidget_h

// QT includes
#include <QBoxLayout>
#include <QWidget>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkPushButton;
class ctkWorkflow;
class ctkWorkflowStep;

class ctkWorkflowButtonBoxWidgetPrivate;

/// \ingroup Widgets
///
/// \brief A widget that controls a workflow
///
/// Creates:
/// - a 'back' button to go to the previous step
/// - a 'next' button to go the next step
/// - 'goTo' buttons to go to finish steps, i.e. those steps in the workflow that do not have any
/// steps following them
///
/// The updateButtons() slot updates the buttons to reflect the current status of the workflow, and
/// should be called whenever the workflow's current step has changed

class CTK_WIDGETS_EXPORT ctkWorkflowButtonBoxWidget : public QWidget
{
  Q_OBJECT

  /// This property controls the text, icon and tooltip of the back button.
  /// "[<-]{backButtonText|\"Back\"}(back:description)" by default.
  /// \sa backButtonFormat(), setBackButtonFormat(),
  /// ctkWorkflow::formatButton(), nextButtonFormat, goToButtonFormat
  Q_PROPERTY(QString backButtonFormat
             READ backButtonFormat WRITE setBackButtonFormat)

  /// This property controls the text, icon and tooltip of the next button.
  /// "{nextButtonText|\"Next\"}(next:description)[->]" by default.
  /// \sa nextButtonFormat(), setNextButtonFormat(),
  /// ctkWorkflow::formatButton(), backButtonFormat, goToButtonFormat
  Q_PROPERTY(QString nextButtonFormat
             READ nextButtonFormat WRITE setNextButtonFormat)

  /// This property controls the text, icon and tooltip of the goTo/finish
  /// button.
  /// "[icon]{stepid|\"Finish\"}" by default.
  /// \sa goToButtonsFormat(), setGoToButtonsFormat(),
  /// ctkWorkflow::formatButton(), backButtonFormat, nextButtonFormat
  Q_PROPERTY(QString goToButtonsFormat
             READ goToButtonsFormat WRITE setGoToButtonsFormat)

  /// This property controls whether the goTo buttons are visible or hidden.
  /// False (visible) by default.
  /// \sa hideInvalidButtons
  Q_PROPERTY(bool hideGoToButtons READ hideGoToButtons WRITE setHideGoToButtons)

  /// This property controls whether the back, next or goTo buttons are hidden when disabled.
  /// Note that buttons can also be hidden via ctkWorkflowWidgetStep::buttonHints.
  /// \sa ctkWofklowWidgetStep::buttonBoxHints
  Q_PROPERTY(bool hideInvalidButtons READ hideInvalidButtons WRITE setHideInvalidButtons)

public:
  typedef QWidget Superclass;
  explicit ctkWorkflowButtonBoxWidget(ctkWorkflow* newWorkflow, QWidget* newParent = 0);
  explicit ctkWorkflowButtonBoxWidget(QWidget* newParent = 0);
  virtual ~ctkWorkflowButtonBoxWidget();

  /// Get the workflow associated with the widget
  ctkWorkflow* workflow()const;

  /// Set the workflow associated with the widget
  void setWorkflow(ctkWorkflow * newWorkflow);

  /// Get the 'back' button
  Q_INVOKABLE ctkPushButton* backButton()const;

  /// Return the backButtonFormat property value.
  /// \sa backButtonFormat, setBackButtonFormat()
  QString backButtonFormat()const;

  /// Set the backButtonFormat property value.
  /// \sa backButtonFormat, backButtonFormat()
  void setBackButtonFormat(const QString& format);

  /// Get the 'next' button
  Q_INVOKABLE ctkPushButton* nextButton()const;

  /// Return the nextButtonFormat property value.
  /// \sa nextButtonFormat, setNextButtonFormat()
  QString nextButtonFormat()const;

  /// Set the nextButtonFormat property value.
  /// \sa nextButtonFormat, nextButtonFormat()
  void setNextButtonFormat(const QString& format);

  /// Return the goToButtonsFormat property value.
  /// \sa goToButtonsFormat, setGoToButtonsFormat()
  QString goToButtonsFormat()const;

  /// Set the goToButtonsFormat property value.
  /// \sa goToButtonsFormat, goToButtonsFormat()
  void setGoToButtonsFormat(const QString& format);

  /// Get a list of the 'goTo' buttons
  QList<ctkPushButton*> goToButtons()const;

  /// Sets the direction of the QBoxLayout that manages this widget (default is
  /// QBoxLayout::LeftToRight)
  QBoxLayout::Direction direction()const;
  void setDirection(const QBoxLayout::Direction& newDirection);

  /// Return the hideGoToButtons property value.
  /// \sa hideGoToButtons
  bool hideGoToButtons()const;
  /// Set the hideGoToButtons property value.
  /// \sa hideGoToButtons
  void setHideGoToButtons(bool hide);

  /// If true, invalid buttons are hidden.  If false, invalid buttons are shown but disabled.
  /// Default is false.
  bool hideInvalidButtons()const;
  void setHideInvalidButtons(bool newHide);

public Q_SLOTS:
  /// Updates the buttons to reflect the current status of the workflow, and should be called
  /// whenever the workflow's current step has changed
  virtual void updateButtons(ctkWorkflowStep* currentStep);

protected Q_SLOTS:
  /// is called when a 'goTo' button is clicked, and retrieves the corresponding goTo step's id to
  /// send to the workflow
  virtual void prepareGoToStep();

protected:
  QScopedPointer<ctkWorkflowButtonBoxWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowButtonBoxWidget);
  Q_DISABLE_COPY(ctkWorkflowButtonBoxWidget);

};

#endif

