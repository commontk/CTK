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

#ifndef __ctkWorkflowAbstractPagedWidget_h
#define __ctkWorkflowAbstractPagedWidget_h

// Qt includes
class QString;

// CTK includes
#include "ctkWorkflowWidget.h"
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkWorkflowStep;

class ctkWorkflowAbstractPagedWidgetPrivate;

/// \ingroup Widgets
/// ctkWorkflowAbstractPagedWidget is the basis for a workflow with a
/// user interface containing multiple pages.
/// By default, each step is placed onto its own page.  Multiple workflow steps can be placed on the
/// same page using associateStepWithPage().
class CTK_WIDGETS_EXPORT ctkWorkflowAbstractPagedWidget : public ctkWorkflowWidget
{
  Q_OBJECT

public:

  typedef ctkWorkflowWidget Superclass;
  explicit ctkWorkflowAbstractPagedWidget(QWidget* newParent = 0);
  virtual ~ctkWorkflowAbstractPagedWidget();

  /// Optionally specify the label to be displayed on the page shown for the given step.
  Q_INVOKABLE virtual void associateStepWithLabel(ctkWorkflowStep* step, QString label);

  /// Dictate that a step be displayed on a particular page index, to override the default setting
  /// to display each encountered step on a new page.  A label can also be provided for the step's
  /// page.
  Q_INVOKABLE virtual void associateStepWithPage(ctkWorkflowStep* step, int index);
  Q_INVOKABLE virtual void associateStepWithPage(ctkWorkflowStep* step, int index, QString label);

  /// Get the workflow group box associated with a particular step
  using ctkWorkflowWidget::workflowGroupBox;
  Q_INVOKABLE virtual ctkWorkflowGroupBox* workflowGroupBox(ctkWorkflowStep* step)const;

protected:

  // Triggers updates of the workflowGroupBox on the current page, when the workflow step has
  // changed.
  virtual void updateStepUI(ctkWorkflowStep* currentStep);

  virtual void initClientArea() = 0;

  virtual QWidget* clientArea() = 0;

  virtual void createNewPage(QWidget* widget) = 0;

  virtual void showPage(QWidget* widget, const QString& label) = 0;

protected:
  QScopedPointer<ctkWorkflowAbstractPagedWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowAbstractPagedWidget);
  Q_DISABLE_COPY(ctkWorkflowAbstractPagedWidget);
};

#endif
