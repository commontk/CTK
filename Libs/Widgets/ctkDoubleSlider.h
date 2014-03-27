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

#ifndef __ctkDoubleSlider_h
#define __ctkDoubleSlider_h

// Qt includes
#include <QSlider>
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include "ctkWidgetsExport.h"

class ctkDoubleSliderPrivate;
class ctkValueProxy;

/// \ingroup Widgets
/// ctkDoubleSlider is a QSlider that controls doubles instead of integers.
/// ctkDoubleSlider internally aggregates a QSlider
/// TODO: ctkDoubleSlider tries to represent a double value with integers. It's
/// of course non-optimal and can lead to errors, it would be better if
/// ctkDoubleSlider works like QDoubleSpinBox, where the value is a QVariant
/// and the conversion between double and integer is only done to convert
/// to/from screen coordinates.
/// \sa ctkRangeSlider, ctkDoubleRangeSlider, ctkRangeWidget
class CTK_WIDGETS_EXPORT ctkDoubleSlider : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged USER true)
  Q_PROPERTY(double sliderPosition READ sliderPosition WRITE setSliderPosition)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(double pageStep READ pageStep WRITE setPageStep)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(double tickInterval READ tickInterval WRITE setTickInterval)
  Q_PROPERTY(QSlider::TickPosition tickPosition READ tickPosition WRITE setTickPosition)
  Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking)
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
  Q_PROPERTY(QString handleToolTip READ handleToolTip WRITE setHandleToolTip)
  Q_PROPERTY(bool invertedAppearance READ invertedAppearance WRITE setInvertedAppearance)
  Q_PROPERTY(bool invertedControls READ invertedControls WRITE setInvertedControls)

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructors, builds a slider whose default values are the same as
  /// QSlider (vertical by default).
  explicit ctkDoubleSlider(QWidget* parent = 0);
  /// Constructors, builds a slider whose default values are the same as
  /// QSlider (vertical by default).
  explicit ctkDoubleSlider(Qt::Orientation orient, QWidget* parent = 0);
  /// Destructor
  virtual ~ctkDoubleSlider();

  /// 
  /// This property holds the sliders's minimum value.
  /// When setting this property, the maximum is adjusted if necessary to
  /// ensure that the range remains valid. Also the slider's current value
  /// is adjusted to be within the new range. 
  void setMinimum(double min);
  double minimum()const;

  /// 
  /// This property holds the slider's maximum value.
  /// When setting this property, the minimum is adjusted if necessary to 
  /// ensure that the range remains valid. Also the slider's current value
  /// is adjusted to be within the new range.
  void setMaximum(double max);
  double maximum()const;

  /// 
  /// Sets the slider's minimum to min and its maximum to max.
  /// If max is smaller than min, min becomes the only legal value.
  void setRange(double min, double max);

  /// 
  /// This property holds the slider's current value.
  /// The slider forces the value to be within the legal range: 
  /// minimum <= value <= maximum.
  /// Changing the value also changes the sliderPosition.
  double value()const;

  /// 
  /// This property holds the single step.
  /// The smaller of two natural steps that an abstract sliders provides and
  /// typically corresponds to the user pressing an arrow key
  /// Default value is 1.
  /// \sa isValidStep()
  void setSingleStep(double step);
  double singleStep()const;

  /// Return true if the step can be handled by the slider, false otherwise.
  /// An invalid step is a step that can't be used to convert from double
  /// to int (too large or too small).
  /// \sa singleStep
  bool isValidStep(double step)const;

  /// 
  /// This property holds the page step.
  /// The larger of two natural steps that an abstract slider provides and
  /// typically corresponds to the user pressing PageUp or PageDown.
  /// Default value is 10.
  void setPageStep(double step);
  double pageStep()const;

  /// 
  /// This property holds the interval between tickmarks.
  /// This is a value interval, not a pixel interval. If it is 0, the slider
  /// will choose between lineStep() and pageStep().
  /// The default value is 0.
  void setTickInterval(double ti);
  double tickInterval()const;

  /// 
  /// This property holds the tickmark position for this slider.
  /// The valid values are described by the QSlider::TickPosition enum.
  /// The default value is QSlider::NoTicks.
  void setTickPosition(QSlider::TickPosition position);
  QSlider::TickPosition tickPosition()const;
  
  /// 
  /// This property holds the current slider position.
  /// If there is no proxy installed and tracking is enabled (the default),
  /// this is identical to value.
  /// With a proxy installed, it allows to modify the proxy value.
  /// \sa value(), setValue(), setValueProxy(), valueProxy()
  double sliderPosition()const;
  void setSliderPosition(double);

  /// 
  /// This property holds whether slider tracking is enabled.
  /// If tracking is enabled (the default), the slider emits the valueChanged()
  /// signal while the slider is being dragged. If tracking is disabled, the 
  /// slider emits the valueChanged() signal only when the user releases the
  /// slider.
  void setTracking(bool enable);
  bool hasTracking()const;
  
  ///
  /// Triggers a slider action. Possible actions are SliderSingleStepAdd, 
  /// SliderSingleStepSub, SliderPageStepAdd, SliderPageStepSub, 
  /// SliderToMinimum, SliderToMaximum, and SliderMove.
  void triggerAction(QAbstractSlider::SliderAction action);

  ///
  /// This property holds the orientation of the slider.
  /// The orientation must be Qt::Vertical (the default) or Qt::Horizontal.
  Qt::Orientation orientation()const;

  /// This property holds whether or not a slider shows its values inverted.
  /// If this property is false (the default), the minimum and maximum will
  /// be shown in its classic position for the inherited widget. If the value
  /// is true, the minimum and maximum appear at their opposite location.
  /// Note: This property makes most sense for sliders and dials. For scroll
  /// bars, the visual effect of the scroll bar subcontrols depends on whether
  /// or not the styles understand inverted appearance; most styles ignore this
  /// property for scroll bars.
  /// \sa invertedControls
  void setInvertedAppearance(bool invertedAppearance);
  bool invertedAppearance()const;

  /// This property holds whether or not the slider inverts its wheel and key
  /// events.
  /// If this property is false, scrolling the mouse wheel "up" and using keys
  /// like page up will increase the slider's value towards its maximum.
  /// Otherwise pressing page up will move value towards the slider's minimum.
  /// \sa invertedAppearance
  void setInvertedControls(bool invertedControls);
  bool invertedControls()const;

  ///
  /// Controls the text to display for the handle tooltip. It is in addition
  /// to the widget tooltip.
  /// "%1" is replaced by the current value of the slider.
  /// Empty string (by default) means no tooltip.
  QString handleToolTip()const;
  void setHandleToolTip(const QString& toolTip);

  /// Reimplemented for internal reasons (handle tooltip).
  virtual bool eventFilter(QObject*, QEvent*);

  /// Return a pointer to the QSlider used internally.
  /// Use with caution.
  /// \sa QSlider
  QSlider* slider()const;

  /// Install or remove a value proxy filter. The value proxy decouples the
  /// displayed value from the value retrieved by the value property.
  /// For example, the value proxy can allow one to display celsius in the
  /// spinbox while the value retrieved from the value property and signals
  /// are in farenheit.
  /// To remove the proxy, simply install a new empty proxy. The proxy
  /// installation/removal is silent.
  /// \sa setValueProxy(), valueProxy()
  void setValueProxy(ctkValueProxy* proxy);
  ctkValueProxy* valueProxy() const;

public Q_SLOTS:
  /// 
  /// This property holds the slider's current value.
  /// The slider forces the value to be within the legal range: 
  /// minimum <= value <= maximum.
  /// Changing the value also changes the sliderPosition.
  void setValue(double value);

  ///
  /// This property holds the orientation of the slider.
  /// The orientation must be Qt::Vertical (the default) or Qt::Horizontal.
  void setOrientation(Qt::Orientation orientation);

Q_SIGNALS:
  ///
  /// This signal is emitted when the slider value has changed, with the new
  /// slider value as argument.
  void valueChanged(double value);

  ///
  /// This signal is emitted when sliderDown is true and the slider moves. 
  /// This usually happens when the user is dragging the slider. The value
  /// is the new slider position.
  /// This signal is emitted even when tracking is turned off.
  void sliderMoved(double position);
  
  ///
  /// This signal is emitted when the user presses the slider with the mouse,
  /// or programmatically when setSliderDown(true) is called.
  void sliderPressed();
  
  /// 
  /// This signal is emitted when the user releases the slider with the mouse, 
  /// or programmatically when setSliderDown(false) is called.
  void sliderReleased();

  ///
  /// This signal is emitted when the slider range has changed, with min being
  /// the new minimum, and max being the new maximum.
  /// Warning: don't confound with valuesChanged(double, double);
  /// \sa QAbstractSlider::rangeChanged()
  void rangeChanged(double min, double max);

protected Q_SLOTS:
  void onValueChanged(int value);
  void onSliderMoved(int position);
  void onRangeChanged(int min, int max);
  void onValueProxyAboutToBeModified();
  void onValueProxyModified();

protected:
  QScopedPointer<ctkDoubleSliderPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDoubleSlider);
  Q_DISABLE_COPY(ctkDoubleSlider);
};

#endif
