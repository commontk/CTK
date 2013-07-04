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

// CTK includes
#include "ctkDoubleSpinBox.h"

// Qt includes
#include <QDoubleSpinBox>


//-----------------------------------------------------------------------------
class ctkQDoubleSpinBox: public QDoubleSpinBox
{
  Q_OBJECT
  /// This property controls whether decreasing the value by the mouse
  /// button or mouse wheel increases the value of the widget, and inverts the
  /// control similarly in the other way round or not. The property is switched off by
  /// default.
  /// \sa invertedControls(), setInvertedControls()
  Q_PROPERTY(bool invertedControls READ invertedControls WRITE setInvertedControls)
public:
  typedef QDoubleSpinBox Superclass;
  ctkQDoubleSpinBox(QWidget* widget);
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

  bool InvertedControls;
private:
  Q_DISABLE_COPY(ctkQDoubleSpinBox);
};

//-----------------------------------------------------------------------------
class ctkDoubleSpinBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleSpinBox);
protected:
  ctkDoubleSpinBox* const q_ptr;
public:
  ctkDoubleSpinBoxPrivate(ctkDoubleSpinBox& object);

  ctkQDoubleSpinBox* SpinBox;
  ctkDoubleSpinBox::SetMode Mode;
  int DefaultDecimals;
  ctkDoubleSpinBox::DecimalsOptions DOption;
  bool InvertedControls;

  void init();
  // Compare two double previously rounded according to the number of decimals
  bool compare(double x1, double x2) const;

  // Set the number of decimals of the spinbox and emit the signal
  // No check if they are the same.
  void setDecimals(int dec);

};
