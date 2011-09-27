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

#ifndef __ctkButton_h
#define __ctkButtoo_h

// QT includes
#include <QPushButton>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkButtonPrivate;

/// QPushButton with a default QIcon "<<"
/// This button has the same behavior as the extension
/// button which appear as the last item in the toolbar
/// ctkButton's purpose is to show, if the button is checked
/// or hide, some others widgets.


class CTK_WIDGETS_EXPORT  ctkButton : public QPushButton
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef QPushButton Superclass;

  explicit ctkButton(QWidget *_parent = 0);
  virtual ~ctkButton();

protected:
  virtual bool event(QEvent *e);

protected:
  QScopedPointer<ctkButtonPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkButton);
  Q_DISABLE_COPY(ctkButton);
};

#endif
