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

#ifndef __ctkExpandButton_h
#define __ctkExpandButton_h

// QT includes
#include <QToolButton>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkExpandButtonPrivate;

/// \ingroup Widgets
/// QToolButton with a standard icon
/// This button has the same behavior as the extension
/// button which appear as the last item in the toolbar
/// ctkButton's purpose is to show, if the button is checked
/// or hide, some others widgets.
/// we can set the orientation of the icon with setOrientation()
/// the widget can also mirror the icon on click if mirrorOnExpand is true.

class CTK_WIDGETS_EXPORT ctkExpandButton
  : public QToolButton
{
  Q_OBJECT
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
  Q_PROPERTY(bool mirrorOnExpand READ mirrorOnExpand WRITE setMirrorOnExpand)
public:
  /// Superclass typedef
  typedef QToolButton Superclass;

  explicit ctkExpandButton(QWidget *_parent = 0);
  virtual ~ctkExpandButton();

  void setMirrorOnExpand(bool newBehavior);
  bool mirrorOnExpand() const;

  void setOrientation(Qt::Orientation newOrientation);
  Qt::Orientation orientation() const;

  virtual QSize sizeHint() const;

private Q_SLOTS:
  void updateIcon(Qt::LayoutDirection newDirection);

protected:
  virtual void nextCheckState();

protected:
  QScopedPointer<ctkExpandButtonPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkExpandButton);
  Q_DISABLE_COPY(ctkExpandButton);
};

#endif
