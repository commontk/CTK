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

#ifndef __ctkMenuButton_h
#define __ctkMenuButton_h

// Qt includes
#include <QPushButton>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkMenuButtonPrivate;

/// \ingroup Widgets
/// Description
/// ctkMenuButton is a QPushButton that separates the clickable area for
/// poping up the optional QMenu from the traditional area of the QPushButton.
/// The menu indicator in the button has its own button and clicking it pops
/// up the menu.
/// ctkMenuButton makes sense only if a QMenu is set.
class CTK_WIDGETS_EXPORT ctkMenuButton : public QPushButton
{
  Q_OBJECT

public:
  ctkMenuButton(QWidget *parent = 0);
  ctkMenuButton(const QString& text, QWidget *parent = 0);
  virtual ~ctkMenuButton();

  /// Reimplemented for internal reasons
  virtual QSize minimumSizeHint()const;
  /// Reimplemented for internal reasons
  virtual QSize sizeHint()const;

protected:
  /// Reimplemented for internal reasons
  virtual void paintEvent(QPaintEvent*);
  /// Reimplemented for internal reasons
  virtual void mousePressEvent(QMouseEvent* event);
  /// Reimplemented for internal reasons
  virtual bool hitButton(const QPoint & pos) const;
  /// Reimplemented for internal reasons
  virtual void initStyleOption ( QStyleOptionButton * option ) const;
protected:
  QScopedPointer<ctkMenuButtonPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkMenuButton);
  Q_DISABLE_COPY(ctkMenuButton);
};

#endif
