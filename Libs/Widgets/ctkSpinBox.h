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

#ifndef __ctkSpinBox_h
#define __ctkSpinBox_h

// Qt includes
#include <QMetaType>
#include <QString>
#include <QWidget>

class QDoubleSpinBox;

// CTK includes
#include "ctkWidgetsExport.h"

class ctkSpinBoxPrivate;

/// \brief Custom SpinBox
/// The ctkSpinBox internaly uses a QDoubleSpinBox while it retain controls
/// over it.
/// \sa ctkDoubleSlider, ctkSliderWidget, ctkRangeSlider
class CTK_WIDGETS_EXPORT ctkSpinBox : public QWidget
{
  Q_OBJECT
  Q_ENUMS(SetMode)
  Q_PROPERTY(SetMode setMode READ setMode WRITE setSetMode)
  Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
  Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)
  Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
  Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)
  Q_PROPERTY(QString cleanText READ cleanText)
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged USER true)

public:

  /// SetMode enums for the spinbox behavior.
  /// SetAlways:
  /// No check is made and the given parameters is directly set
  /// on the internal QDoubleSpinBox.
  /// SetIfDifferent:
  /// Default mode, the given parameter is checked agains the
  /// current internal value and only set if they are different.
  /// For double, the comparison is based on the input parameters rounded
  /// with the current number of decimals (see round()).
  /// \sa setMode(), setSetMode(), round()
  enum SetMode
    {
    SetAlways,
    SetIfDifferent,
    };

  typedef QWidget Superclass;

  /// Constructor, creates a ctkSpinBox. The look and feel
  /// are the same as of a QDoubleSpinBox
  ctkSpinBox(QWidget* parent = 0);
  ctkSpinBox(ctkSpinBox::SetMode mode, QWidget* parent = 0);

  /// Get the spinbox current value
  /// \sa setValue(), cleanText()
  double value() const;

  /// Get the spinbox current displayed value
  /// \sa value(), cleanText()
  double displayedValue() const;

  /// Get the spinbox current text. This includes any prefix or suffix.
  /// \sa prefix(), suffix()
  QString text() const;

  /// Get the spinbox current text. This excludes any prefix or suffix.
  /// \sa value()
  QString cleanText() const;

  /// Set/Get the spinbox alignement
  Qt::Alignment alignment () const;
  void setAlignment (Qt::Alignment flag);

  /// Set/Get the frame
  void setFrame(bool frame);
  bool hasFrame() const;

  /// Add/Get a prefix to the displayed value. For example, one might want to
  /// add the $ sign.
  /// \sa suffix(), text()
  QString prefix() const;
  void setPrefix(const QString &prefix);

  /// Add/Get a suffix to the displayed value. For example, one might want to
  /// add the F (fahrenheit) sign.
  /// \sa prefix(), text()
  QString suffix() const;
  void setSuffix(const QString &suffix);

  /// Set/Get the single step. This represents by how much the value will
  /// decrease or increase when clicking the spinbox arrow or using stepUp or
  /// stepDown(). Default is 1.0.
  /// \sa setUp(), stepDown(), setDecimals().
  double singleStep() const;
  void setSingleStep(double value);

  /// Set/Get the range of the spinbox. Default range is [0.0, 9.9].
  double minimum() const;
  void setMinimum(double min);
  double maximum() const;
  void setMaximum(double max);
  void setRange(double min, double max);

  /// Set/Get number of decimals displayed. For a spinbox dealing only with
  /// integers, set this to 0.
  /// \sa addOneDecimal(), removeOneDecimal()
  int decimals() const;
  void setDecimals(int decimal);

  /// Returns the rounded value according to the number of decimals of
  /// the spinbox.
  /// \sa decimals()
  double round(double value) const;

  /// Get a pointer on the spinbox used internally. It can be useful to
  /// change display properties for example. To use with caution.
  /// \sa QDoubleSpinBox::QDoubleSpinBox
  QDoubleSpinBox* spinBox() const;

  /// Set the spinbox mode when using a set*() method.
  //// \sa round(), setValue(), setValueIfDifferent(), setValueAlways()
  ctkSpinBox::SetMode setMode() const;
  void setSetMode(SetMode mode);

public Q_SLOTS:
  /// Set the value of the spinbox following the current mode.
  /// \sa setMode(), value(), setValueIfDifferent(), setValueAlways()
  void setValue(double value);

  /// Set the value of the spinbox followin the SetIfDifferent mode.
  /// \sa value(), setValue(), setMode(), setValueAlways()
  void setValueIfDifferent(double value);

  /// Set the value of the spinbox following the SetAlways mode.
  /// \sa value(), setValue(), setMode(), setValueIfDifferent()
  void setValueAlways(double value);

  /// Increase/Decrease the current value by a single step.
  /// \sa value(), singleStep()
  void stepUp();
  void stepDown();

Q_SIGNALS:
  /// Emitted everytime the spinbox value is modified
  /// \sa QDoubleSpinBox::valueChanged()
  void valueChanged(double);
  void valueChanged(const QString &);

  /// Simple broadcast of the QAbstractSpinbox::editingFinished
  /// \sa QAbstractSpinbox::editingFinished
  void editingFinished();

protected:
  ctkSpinBoxPrivate* const d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSpinBox);
  Q_DISABLE_COPY(ctkSpinBox);
};

Q_DECLARE_METATYPE(ctkSpinBox::SetMode)

#endif //__ctkSpinBox_h
