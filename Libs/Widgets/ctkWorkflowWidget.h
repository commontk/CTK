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

#ifndef __ctkWorkflowWidget_h
#define __ctkWorkflowWidget_h

// Qt includes
#include <QWidget>
class QPushButton; 
class QGroupBox;
#include <QBoxLayout>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkWorkflow;
class ctkWorkflowStep;
class ctkWorkflowButtonBoxWidget;
class ctkWorkflowGroupBox;

class ctkWorkflowWidgetPrivate;

/// \brief ctkWorkflowWidget is the basis for a workflow with a user interface.  It groups together
/// and manages a ctkWorkflowGroupBox (to display the step) and a ctkWorkflowButtonBoxWidget
/// (providing buttons for traversing the workflow).
class CTK_WIDGETS_EXPORT ctkWorkflowWidget : public QWidget
{
  Q_OBJECT

public:

  typedef QWidget Superclass;
  explicit ctkWorkflowWidget(QWidget* parent = 0);
  virtual ~ctkWorkflowWidget();

  /// Set/get the workflow associated with this widget.
  virtual ctkWorkflow* workflow()const;
  virtual void setWorkflow(ctkWorkflow* newWorkflow);

  /// Get the widget constaining the title, subtitle, pre-text, post-text, error-text and client area
  /// layout.
  virtual ctkWorkflowGroupBox* workflowGroupBox()const;

  /// Set/get whether or not to associate a buttonBoxWidget with this step (default true)
  bool showButtonBoxWidget()const;
  void setShowButtonBoxWidget(bool newShowButtonBoxWidget);

  /// Get the widget with the 'next', 'back' and 'goTo' buttons
  ctkWorkflowButtonBoxWidget* buttonBoxWidget()const;

public slots:
  /// Triggers updates of the the workflowGroupBox and the buttonBoxWidget when the current workflow
  /// step has changed.
  virtual void onCurrentStepChanged(ctkWorkflowStep* currentStep);

protected:

  // Triggers updates of the workflowGroupBox when the current workflow step has changed.
  virtual void updateStepUI(ctkWorkflowStep* currentStep);

  // Triggers updates of the buttonBoxWidget when the current workflow step has changed.
  void updateButtonBoxUI(ctkWorkflowStep* currentStep);
 
protected:
  QScopedPointer<ctkWorkflowWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowWidget);
  Q_DISABLE_COPY(ctkWorkflowWidget);

};

#endif

