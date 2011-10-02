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

#ifndef __ctkCheckablePushButton_h
#define __ctkCheckablePushButton_h

// Qt includes
#include <QPushButton>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkCheckablePushButtonPrivate;

/// Description
/// ctkCheckablePushButton is a QPushButton with a checkbox. By default
/// the checkbox is connected to the checkable property of the push button.
/// You can change this behaviour by clearing the "checkBoxControlsButton"
/// property.
/// The checkBoxUserCheckable property determines if the state of the
/// checkbox can be changed interactively.
class CTK_WIDGETS_EXPORT ctkCheckablePushButton : public QPushButton
{
  Q_OBJECT
  Q_PROPERTY(Qt::Alignment buttonTextAlignment READ buttonTextAlignment WRITE setButtonTextAlignment)
  Q_PROPERTY(Qt::Alignment indicatorAlignment READ indicatorAlignment WRITE setIndicatorAlignment)
  Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY checkStateChanged)
  Q_PROPERTY(bool checkBoxControlsButton READ checkBoxControlsButton WRITE setCheckBoxControlsButton)
  Q_PROPERTY(bool checkBoxUserCheckable READ isCheckBoxUserCheckable WRITE setCheckBoxUserCheckable)

public:
  ctkCheckablePushButton(QWidget *parent = 0);
  ctkCheckablePushButton(const QString& text, QWidget *parent = 0);
  virtual ~ctkCheckablePushButton();

  ///
  /// Set the alignment of the text on the button,
  /// Qt::Left|Qt::VCenter by default.
  void setButtonTextAlignment(Qt::Alignment textAlignment);
  Qt::Alignment buttonTextAlignment()const;

  ///
  /// Set the alignment of the indicator (arrow) on the button,
  /// Qt::Left|Qt::VCenter by default.
  void setIndicatorAlignment(Qt::Alignment indicatorAlignment);
  Qt::Alignment indicatorAlignment()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

  virtual Qt::CheckState checkState()const;
  virtual void setCheckState(Qt::CheckState checkState);

  virtual bool checkBoxControlsButton()const;
  virtual void setCheckBoxControlsButton(bool b);

  virtual bool isCheckBoxUserCheckable()const;
  virtual void setCheckBoxUserCheckable(bool b);

signals:
  void checkStateChanged(Qt::CheckState newCheckState);

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
  QScopedPointer<ctkCheckablePushButtonPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCheckablePushButton);
  Q_DISABLE_COPY(ctkCheckablePushButton);
};

#endif
