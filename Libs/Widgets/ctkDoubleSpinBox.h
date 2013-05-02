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

#ifndef __ctkDoubleSpinBox_h
#define __ctkDoubleSpinBox_h

// QT includes
#include <QDoubleSpinBox>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkDoubleSpinBoxPrivate;

/// \ingroup Widgets
/// ctkDoubleSpinBox is an advanced QDoubleSpinBox with the invertedControls
/// property. If the property is set, the decreasing the value by the mouse
/// button or mouse wheel increase the value of the widget, and inverts the
/// control similarly in the other way round. The property is switched off by
/// default.
class CTK_WIDGETS_EXPORT ctkDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
  Q_PROPERTY(bool invertedControls READ invertedControls WRITE setInvertedControls)

public:
  typedef QDoubleSpinBox Superclass;

  ctkDoubleSpinBox(QWidget *_parent = 0);
  virtual ~ctkDoubleSpinBox();

  /// This property holds whether or not the spin box inverts its wheel and key
  /// events.
  /// If this property is false, scrolling the mouse wheel "up" and using keys
  /// like page up will increase the spinbox's value towards its maximum.
  /// Otherwise pressing page up will move value towards the slider's minimum.
  void setInvertedControls(bool invertedControls);
  bool invertedControls() const;

  /// Overrides QDoubleSpinBox::stepBy(int) and negates the step number if the
  /// invertedControls property is true.
  virtual void stepBy(int steps);

protected:
  /// If the invertedControls property is false (by default) then this function
  /// behavesLike QDoubleSpinBox::stepEnabled(). If the property is true then
  /// stepping down is allowed if the value is less then the maximum, and
  /// stepping up is allowed if the value is greater then the minimum.
  virtual StepEnabled stepEnabled () const;

  QScopedPointer<ctkDoubleSpinBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDoubleSpinBox);
  Q_DISABLE_COPY(ctkDoubleSpinBox);
};

#endif
