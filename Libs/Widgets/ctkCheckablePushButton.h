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

// CTK includes
#include <ctkPimpl.h>
#include "ctkPushButton.h"
#include "ctkWidgetsExport.h"

class ctkCheckablePushButtonPrivate;

/// \ingroup Widgets
/// Description
/// ctkCheckablePushButton is a QPushButton with a checkbox.
///
/// \warning The checkbox is drawn in place of the pushbuton icon, any icon
/// will then be ignored.
class CTK_WIDGETS_EXPORT ctkCheckablePushButton : public ctkPushButton
{
  Q_OBJECT
  /// This property controls the location of the checkbox with regard to the text.
  /// Qt::AlignLeft|Qt::AlignVCenter by default
  Q_PROPERTY(Qt::Alignment indicatorAlignment READ indicatorAlignment WRITE setIndicatorAlignment)
  Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY checkStateChanged)
  Q_PROPERTY(bool checkBoxControlsButton READ checkBoxControlsButton WRITE setCheckBoxControlsButton)
  Q_PROPERTY(bool checkBoxControlsButtonToggleState READ checkBoxControlsButtonToggleState WRITE setCheckBoxControlsButtonToggleState)
  Q_PROPERTY(bool checkBoxUserCheckable READ isCheckBoxUserCheckable WRITE setCheckBoxUserCheckable)

public:
  ctkCheckablePushButton(QWidget *parent = 0);
  ctkCheckablePushButton(const QString& text, QWidget *parent = 0);
  virtual ~ctkCheckablePushButton();

  /// Set the alignment of the indicator (arrow) on the button,
  /// Qt::AlignLeft|Qt::AlignVCenter by default.
  void setIndicatorAlignment(Qt::Alignment indicatorAlignment);
  Qt::Alignment indicatorAlignment()const;

  /// Get checked state of the checkbox on the button.
  virtual Qt::CheckState checkState()const;
  /// Set checked state of the checkbox on the button.
  virtual void setCheckState(Qt::CheckState checkState);

  /// By default the checkbox is connected to the checkable property of the push button.
  /// You can change this behaviour by clearing the "checkBoxControlsButton"
  /// property.
  /// \note In checkBoxControlsButton mode, calling setCheckable() instead of
  /// setCheckState() may not refresh the button automatically. Use setCheckState()
  /// instead.
  /// \note You can automatically check the button when the user checks the
  /// checkbox by connecting the checkBoxToggled(bool) signal with the
  /// setChecked(bool) slot or by enabling "checkBoxControlsButtonToggleState" property.
  virtual bool checkBoxControlsButton()const;
  virtual void setCheckBoxControlsButton(bool b);

  /// If both checkBoxControlsButton and checkBoxControlsButtonToggleState
  /// are enabled then check state is synchronized with pushed state of the button
  /// (checking the checkbox also pushes down the button and releasing the button
  /// unchecks the checkbox).
  virtual bool checkBoxControlsButtonToggleState()const;
  virtual void setCheckBoxControlsButtonToggleState(bool b);

  /// The checkBoxUserCheckable property determines if the state of the
  /// checkbox can be changed interactively by the user by clicking on the
  /// checkbox.
  virtual bool isCheckBoxUserCheckable()const;
  virtual void setCheckBoxUserCheckable(bool b);

Q_SIGNALS:
  /// Fired anytime the checkbox change of state
  void checkBoxToggled(bool);
  /// Fired anytime the checkbox change of state
  void checkStateChanged(Qt::CheckState newCheckState);

protected:
  /// Reimplemented for internal reasons
  virtual void mousePressEvent(QMouseEvent* event);
  /// Reimplemented for internal reasons
  virtual bool hitButton(const QPoint & pos) const;
  /// Reimplemented for internal reasons
  void checkStateSet() override;
  /// Reimplemented for internal reasons
  void nextCheckState() override;
private:
  Q_DECLARE_PRIVATE(ctkCheckablePushButton);
  Q_DISABLE_COPY(ctkCheckablePushButton);
};

#endif
