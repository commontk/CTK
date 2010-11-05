/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkRangeWidget_h
#define __ctkRangeWidget_h

// Qt includes
#include <QSlider>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkDoubleRangeSlider;
class ctkRangeWidgetPrivate;

///
/// ctkRangeWidget is a wrapper around a ctkDoubleRangeSlider and 2 QSpinBoxes
/// \image html http://www.commontk.org/images/1/14/CtkRangeWidget.png
/// \sa ctkSliderSpinBoxWidget, ctkDoubleRangeSlider, QSpinBox
class CTK_WIDGETS_EXPORT ctkRangeWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(double minimumValue READ minimumValue WRITE setMinimumValue)
  Q_PROPERTY(double maximumValue READ maximumValue WRITE setMaximumValue)
  Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
  Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)
  Q_PROPERTY(double tickInterval READ tickInterval WRITE setTickInterval)
  Q_PROPERTY(bool autoSpinBoxWidth READ isAutoSpinBoxWidth WRITE setAutoSpinBoxWidth)
  Q_PROPERTY(Qt::Alignment spinBoxTextAlignment READ spinBoxTextAlignment WRITE setSpinBoxTextAlignment)
  Q_PROPERTY(Qt::Alignment spinBoxAlignment READ spinBoxAlignment WRITE setSpinBoxAlignment)
  Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking)
  Q_PROPERTY(bool symmetricMoves READ symmetricMoves WRITE setSymmetricMoves)

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  /// If \li parent is null, ctkRangeWidget will be a top-leve widget
  /// \note The \li parent can be set later using QWidget::setParent()
  explicit ctkRangeWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkRangeWidget();

  ///
  /// This property holds the sliders and spinbox minimum value.
  /// FIXME: Test following specs.
  /// When setting this property, the maximum is adjusted if necessary
  /// to ensure that the range remains valid.
  /// Also the slider's current value is adjusted to be within the new range.
  double minimum()const;
  void setMinimum(double minimum);

  ///
  /// This property holds the sliders and spinbox minimum value.
  /// FIXME: Test following specs.
  /// When setting this property, the maximum is adjusted if necessary
  /// to ensure that the range remains valid.
  /// Also the slider's current value is adjusted to be within the new range.
  double maximum()const;
  void setMaximum(double maximum);
  /// Description
  /// Utility function that set the min/max in once
  void setRange(double min, double max);

  ///
  /// This property holds the slider and spinbox minimum value.
  /// ctkRangeWidget forces the value to be within the
  /// legal range: minimum <= minimumValue <= maximumValue <= maximum.
  double minimumValue()const;

  ///
  /// This property holds the slider and spinbox maximum value.
  /// ctkRangeWidget forces the value to be within the
  /// legal range: minimum <= minimumValue <= maximumValue <= maximum.
  double maximumValue()const;

  ///
  /// Utility function that returns both values at the same time
  /// Returns minimumValue and maximumValue
  void values(double &minValue, double &maxValue)const;

  ///
  /// This property holds the single step.
  /// The smaller of two natural steps that the
  /// slider provides and typically corresponds to the
  /// user pressing an arrow key.
  double singleStep()const;
  void setSingleStep(double step);

  ///
  /// This property holds the precision of the spin box, in decimals.
  /// Sets how many decimals the spinbox will use for displaying and
  /// interpreting doubles.
  int decimals()const;
  void setDecimals(int decimals);

  ///
  /// This property holds the spin box's prefix.
  /// The prefix is prepended to the start of the displayed value.
  /// Typical use is to display a unit of measurement or a currency symbol
  QString prefix()const;
  void setPrefix(const QString& prefix);

  ///
  /// This property holds the spin box's suffix.
  /// The suffix is appended to the end of the displayed value.
  /// Typical use is to display a unit of measurement or a currency symbol
  QString suffix()const;
  void setSuffix(const QString& suffix);

  ///
  /// This property holds the interval between tickmarks.
  /// This is a value interval, not a pixel interval.
  /// If it is 0, the slider will choose between lineStep() and pageStep().
  /// The default value is 0.
  double tickInterval()const;
  void setTickInterval(double ti);

  ///
  /// This property holds the alignment of the spin boxes.
  /// Possible Values are Qt::AlignTop, Qt::AlignBottom, and Qt::AlignVCenter.
  /// By default, the alignment is Qt::AlignVCenter
  void setSpinBoxAlignment(Qt::Alignment alignment);
  Qt::Alignment spinBoxAlignment()const;

  ///
  /// This property holds the alignment of the text inside the spin boxes.
  /// Possible Values are Qt::AlignLeft, Qt::AlignRight, and Qt::AlignHCenter.
  /// By default, the alignment is Qt::AlignLeft
  void setSpinBoxTextAlignment(Qt::Alignment alignment);
  Qt::Alignment spinBoxTextAlignment()const;

  ///
  /// This property holds whether slider tracking is enabled.
  /// If tracking is enabled (the default), the widget emits the valueChanged()
  /// signal while the slider or spinbox is being dragged. If tracking is
  /// disabled, the widget emits the valueChanged() signal only when the user
  /// releases the slider or spinbox.
  void setTracking(bool enable);
  bool hasTracking()const;

  ///
  /// Set/Get the auto spinbox width
  /// When the autoSpinBoxWidth property is on, the width of the SpinBox is
  /// set to the same width of the largest QSpinBox of its
  // ctkRangeWidget siblings.
  bool isAutoSpinBoxWidth()const;
  void setAutoSpinBoxWidth(bool autoWidth);
  
  ///
  /// When symmetricMoves is true, moving a handle will move the other handle
  /// symmetrically, otherwise the handles are independent. False by default
  bool symmetricMoves()const;
  void setSymmetricMoves(bool symmetry);

public slots:
  ///
  /// Reset the slider and spinbox to zero (value and position)
  void reset();
  void setMinimumValue(double value);
  void setMaximumValue(double value);
  ///
  /// Utility function that set the min and max values at once
  void setValues(double minValue, double maxValue);

signals:
  /// Use with care:
  /// sliderMoved is emitted only when the user moves the slider
  //void sliderMoved(double position);
  void minimumValueChanged(double value);
  void minimumValueIsChanging(double value);
  void maximumValueChanged(double value);
  void maximumValueIsChanging(double value);
  void valuesChanged(double minValue, double maxValue);
  void rangeChanged(double min, double max);

protected slots:
  void startChanging();
  void stopChanging();
  void changeValues(double newMinValue, double newMaxValue);
  void changeMinimumValue(double value);
  void changeMaximumValue(double value);
  void setMinimumToMaximumSpinBox(double minimum);
  void setMaximumToMinimumSpinBox(double maximum);
  void onSliderRangeChanged(double min, double max);

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);

  /// can be used to change the slider by a custom one
  ctkDoubleRangeSlider* slider()const;
  void setSlider(ctkDoubleRangeSlider* slider);

protected:
  QScopedPointer<ctkRangeWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkRangeWidget);
  Q_DISABLE_COPY(ctkRangeWidget);

};

#endif
