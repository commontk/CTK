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

#ifndef __ctkWorkflowButtonBoxWidget_h
#define __ctkWorkflowButtonBoxWidget_h 

// QT includes
#include <QWidget>
//class QList;
class QPushButton;
#include <QBoxLayout>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkWorkflow;
class ctkWorkflowStep;

class ctkWorkflowButtonBoxWidgetPrivate;

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

  Q_PROPERTY(QString backButtonDefaultText
             READ backButtonDefaultText WRITE setBackButtonDefaultText)
  Q_PROPERTY(QString nextButtonDefaultText
             READ nextButtonDefaultText WRITE setNextButtonDefaultText)
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
  QPushButton* backButton()const;

  /// Get 'back' button default text
  QString backButtonDefaultText()const;

  /// \brief Set 'back' button \a defaultText
  /// \a defaultText is used if the text associated with the current step is empty
  void setBackButtonDefaultText(const QString& defaultText);

  /// Get the 'next' button
  QPushButton* nextButton()const;

  /// Get 'next' button default text
  QString nextButtonDefaultText()const;

  /// \brief Set 'next' button \a defaultText
  /// \a defaultText is used if the text associated with the current step is empty
  void setNextButtonDefaultText(const QString& defaultText);

  /// Get a list of the 'goTo' buttons
  QList<QPushButton*> goToButtons()const;

  /// Sets the direction of the QBoxLayout that manages this widget (default is
  /// QBoxLayout::LeftToRight)
  QBoxLayout::Direction direction()const;
  void setDirection(const QBoxLayout::Direction& newDirection);

  /// If true, invalid buttons are hidden.  If false, invalid buttons are shown but disabled.
  /// Default is false.
  bool hideInvalidButtons()const;
  void setHideInvalidButtons(bool newHide);

public slots:
  /// Updates the buttons to reflect the current status of the workflow, and should be called
  /// whenever the workflow's current step has changed
  virtual void updateButtons(ctkWorkflowStep* currentStep);

protected slots:
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

