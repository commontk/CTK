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

#ifndef __ctkWorkflowWidget_h
#define __ctkWorkflowWidget_h

// Qt includes
#include <QWidget>
#include <QVariant>
class QAbstractButton;

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkWorkflow;
class ctkWorkflowStep;
class ctkWorkflowButtonBoxWidget;
class ctkWorkflowGroupBox;
class ctkWorkflowWidgetStep;

class ctkWorkflowWidgetPrivate;

/// \ingroup Widgets
/// \brief ctkWorkflowWidget is the basis for a workflow with a user interface.  It groups together
/// and manages a ctkWorkflowGroupBox (to display the step) and a ctkWorkflowButtonBoxWidget
/// (providing buttons for traversing the workflow).
class CTK_WIDGETS_EXPORT ctkWorkflowWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(bool showButtonBoxWidget READ showButtonBoxWidget WRITE setShowButtonBoxWidget)
public:

  typedef QWidget Superclass;
  explicit ctkWorkflowWidget(QWidget* parent = 0);
  virtual ~ctkWorkflowWidget();

  /// Set/get the workflow associated with this widget.
  Q_INVOKABLE virtual ctkWorkflow* workflow()const;
  Q_INVOKABLE virtual void setWorkflow(ctkWorkflow* newWorkflow);

  Q_INVOKABLE ctkWorkflowWidgetStep* widgetStep(const QString& id)const;

  /// Get the widget constaining the title, subtitle, pre-text, post-text, error-text and client area
  /// layout.
  Q_INVOKABLE virtual ctkWorkflowGroupBox* workflowGroupBox()const;

  /// Set/get whether or not to associate a buttonBoxWidget with this step (default true)
  bool showButtonBoxWidget()const;
  void setShowButtonBoxWidget(bool newShowButtonBoxWidget);

  /// Get the widget with the 'next', 'back' and 'goTo' buttons
  Q_INVOKABLE ctkWorkflowButtonBoxWidget* buttonBoxWidget()const;

  /// Apply the text, icon and tooltip format to the button.
  ///  * {PROP}, [prop] or (PROP): value of the PROP property (e.g. stepid,
  /// name, description...) used as button text, icon or tooltip respectively.
  /// PROP can be prefixed by 'back:', 'next:' or 'current:',
  /// the property will then be the one of the previous, next or current step.
  ///  * [<-]: Back arrow icon. If it is the first item, the icon is to the left
  /// of the button text.
  ///  * [->]: Next arrow icon. If it is the last item, the icon is to the right
  ///  of the button text if the button is a ctkPushButton.
  ///  * {#} or (#): 1-based index of the step (int)
  ///  * {!#} or {!#} : Total number of steps (int)
  ///  * "ABCD": text for the button
  ///  * {PROP|"ABCD"}: Use ABCD as fallback if PROP is not a valid property or
  /// if the text is empty.
  ///
  /// Examples:
  ///   "{next:#}"/"{!#}") "{next:name}(next:description)[->]" will format the button with:
  ///  * text="3/3) Compute Algorithm" if the next step is the last step of a
  /// 3-step-workflow, and its name is "Compute Algorithm".
  ///  * icon=QStyle::SP_ArrowRight
  ///  * tooltip="This step computes the algorithm" if the next step description
  /// is "This step...".
  /// \sa parse(), formatText()
  static void formatButton(QAbstractButton* button, const QString& format, ctkWorkflowWidgetStep* step);
  /// Return the text contained in \a format.
  /// \sa parse(), formatButton()
  static QString formatText(const QString& format, ctkWorkflowWidgetStep* step);

public Q_SLOTS:
  /// Triggers updates of the the workflowGroupBox and the buttonBoxWidget when the current workflow
  /// step has changed.
  virtual void onCurrentStepChanged(ctkWorkflowStep* currentStep);

protected Q_SLOTS:
  void onStepRegistered(ctkWorkflowStep* step);

protected:

  // Triggers updates of the workflowGroupBox when the current workflow step has changed.
  virtual void updateStepUI(ctkWorkflowStep* currentStep);

  // Triggers updates of the buttonBoxWidget when the current workflow step has changed.
  void updateButtonBoxUI(ctkWorkflowStep* currentStep);

  /// Return the value of the formatItem.
  /// \sa format()
  static QVariant buttonItem(QString formatItem, ctkWorkflowWidgetStep* step);
  /// Return a dictionary of formats. Keys can be 'text', 'icon', 'iconalignment' or 'tooltip'.
  /// \sa buttonItem(), formatButton(), formatText()
  static QMap<QString, QVariant> parse(const QString& format, ctkWorkflowWidgetStep* step);

protected:
  QScopedPointer<ctkWorkflowWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowWidget);
  Q_DISABLE_COPY(ctkWorkflowWidget);

};

#endif

