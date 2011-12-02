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

#ifndef __ctkTemplateWidget_h
#define __ctkTemplateWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkTemplateWidgetPrivate;

/// \ingroup Widgets
///
/// ctkTemplateWidget allows the user to ...
class CTK_WIDGETS_EXPORT ctkTemplateWidget : public QWidget
{
  Q_OBJECT

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  /// If \li parent is null, ctkTemplateWidget will be a top-level widget
  /// \note The \li parent can be set later using QWidget::setParent()
  explicit ctkTemplateWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkTemplateWidget();

public Q_SLOTS:

Q_SIGNALS:

protected:
  QScopedPointer<ctkTemplateWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTemplateWidget);
  Q_DISABLE_COPY(ctkTemplateWidget);
};

#endif
