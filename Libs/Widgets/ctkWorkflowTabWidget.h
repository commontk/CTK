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

#ifndef __ctkWorkflowTabWidget_h
#define __ctkWorkflowTabWidget_h

// Qt includes
class QWidget;
class QString;

// CTK includes
#include "ctkWorkflowAbstractPagedWidget.h"
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkWorkflowTabWidgetPrivate;

/// \ingroup Widgets
///
/// \brief ctkWorkflowTabWidget is the basis for a workflow with a user
class CTK_WIDGETS_EXPORT ctkWorkflowTabWidget : public ctkWorkflowAbstractPagedWidget
{
  Q_OBJECT

public:
  typedef ctkWorkflowAbstractPagedWidget Superclass;

  explicit ctkWorkflowTabWidget(QWidget* parent = 0);
  virtual ~ctkWorkflowTabWidget();

  virtual void createNewPage(QWidget* widget);

  virtual void showPage(QWidget* widget, const QString& label);

  virtual void initClientArea();

  virtual QWidget* clientArea();

protected:
  QScopedPointer<ctkWorkflowTabWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowTabWidget);
  Q_DISABLE_COPY(ctkWorkflowTabWidget);
};

#endif
