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

#ifndef __ctkSliderWidget_h
#define __ctkSliderWidget_h

// Qt includes
#include <QSlider>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkDoubleSlider;
class ctkPopupWidget;
class ctkSliderWidgetPrivate;
class ctkDoubleSpinBox;

/// \ingroup Widgets
///
/// ctkSliderWidget is a wrapper around a ctkDoubleSlider and a ctkDoubleSpinBox
/// where the slider value and the spinbox value are synchronized.
/// \sa ctkRangeWidget, ctkDoubleRangeSlider, QDoubleSpinBox
class CTK_WIDGETS_EXPORT ctkSliderWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(double pageStep READ pageStep WRITE setPageStep)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(double value READ value WRITE setValue)
  Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
  Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)
  Q_PROPERTY(double tickInterval READ tickInterval WRITE setTickInterval)
  Q_PROPERTY(QSlider::TickPosition tickPosition READ tickPosition WRITE setTickPosition)
  Q_FLAGS(SynchronizeSiblings)
  Q_PROPERTY(SynchronizeSiblings SynchronizeSibling READ synchronizeSiblings WRITE setSynchronizeSiblings)
  Q_PROPERTY(Qt::Alignment spinBoxAlignment READ spinBoxAlignment WRITE setSpinBoxAlignment)
  Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking)
  Q_PROPERTY(bool spinBoxVisible READ isSpinBoxVisible WRITE setSpinBoxVisible);
  Q_PROPERTY(bool popupSlider READ hasPopupSlider WRITE setPopupSlider);
  Q_PROPERTY(bool invertedAppearance READ invertedAppearance WRITE setInvertedAppearance)
  Q_PROPERTY(bool invertedControls READ invertedControls WRITE setInvertedControls)

public:

  /// Synchronize properties of the slider siblings:
  /// NoSynchronize:
  /// The slider widget siblings aren't updated and this widget does not update
  /// from its siblings.
  /// SynchronizeWidth:
  /// The width of the SpinBox is set to the same width of the largest QDoubleSpinBox
  /// of its ctkSliderWidget siblings.
  /// SynchronizeDecimals:
  /// Whenever one of the siblings changes its number of decimals, all its
  /// siblings Synchronize to the new number of decimals.
  ///
  /// Default is SynchronizeWidth |SynchronizeDecimals.
  /// \sa SynchronizeSiblings(), setSynchronizeSiblings()
  enum SynchronizeSibling
    {
    NoSynchronize = 0x000,
    SynchronizeWidth = 0x001,
    SynchronizeDecimals = 0x002,
    };
  Q_DECLARE_FLAGS(SynchronizeSiblings, SynchronizeSibling)

  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructors
  explicit ctkSliderWidget(QWidget* parent = 0);
  virtual ~ctkSliderWidget();

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
  /// This property holds the current slider position.
  /// If tracking is enabled (the default), this is identical to value.
  //double sliderPosition()const;
  //void setSliderPosition(double position);

  /// 
  /// This property holds the slider and spinbox current value.
  /// ctkSliderWidget forces the value to be within the
  /// legal range: minimum <= value <= maximum.
  double value()const;

  /// 
  /// This property holds the single step.
  /// The smaller of two natural steps that the 
  /// slider provides and typically corresponds to the 
  /// user pressing an arrow key.
  double singleStep()const;
  void setSingleStep(double step);

  /// 
  /// This property holds the page step.
  /// The larger of two natural steps that an abstract slider provides and
  /// typically corresponds to the user pressing PageUp or PageDown.
  double pageStep()const;
  void setPageStep(double step);

  /// 
  /// This property holds the precision of the spin box, in decimals.
  int decimals()const;

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
  void setTickInterval(double tick);

  /// 
  /// This property holds the tickmark position for the slider.
  /// The valid values are described by the QSlider::TickPosition enum.
  /// The default value is QSlider::NoTicks.
  void setTickPosition(QSlider::TickPosition position);
  QSlider::TickPosition tickPosition()const;
  
  /// 
  /// This property holds the alignment of the spin box.
  /// Possible Values are Qt::AlignLeft, Qt::AlignRight, and Qt::AlignHCenter.
  /// By default, the alignment is Qt::AlignLeft
  void setSpinBoxAlignment(Qt::Alignment alignment);
  Qt::Alignment spinBoxAlignment()const;

  /// 
  /// This property holds whether slider tracking is enabled.
  /// If tracking is enabled (the default), the widget emits the valueChanged() 
  /// signal while the slider or spinbox is being dragged. If tracking is 
  /// disabled, the widget emits the valueChanged() signal only when the user 
  /// releases the slider or spinbox.
  void setTracking(bool enable);
  bool hasTracking()const;

  /// 
  /// Set/Get the synchronize siblings mode. This helps when having multiple
  /// ctkSliderWidget stacked upon each other.
  /// Default flag is SynchronizeWidth | SynchronizeDecimals.
  /// \sa SynchronizeSiblingsModes
  ctkSliderWidget::SynchronizeSiblings synchronizeSiblings() const;
  void setSynchronizeSiblings(ctkSliderWidget::SynchronizeSiblings options);

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

  /// This property holds whether or not the slider and the spinbox invert
  /// their wheel and key events.
  /// If this property is false, scrolling the mouse wheel "up" and using keys
  /// like page up will increase the value of the slider widget towards its
  /// maximum. Otherwise, pressing page up will move value towards the minimum.
  /// The default value of the property is false.
  /// \sa invertedAppearance
  void setInvertedControls(bool invertedControls);
  bool invertedControls()const;

  ///
  /// The Spinbox visibility can be controlled using setSpinBoxVisible() and
  /// isSpinBoxVisible().
  bool isSpinBoxVisible()const;

  ///
  /// The slider can be handled as a popup for the spinbox. The location where
  /// the popup appears is controlled by \sa alignement.
  /// False by default.
  /// Note: some sizing issues in the popup can happen if the ctkSliderWidget
  /// has already parent. You might want to consider setting this property
  /// before setting a parent to ctkSliderWidget.
  bool hasPopupSlider()const;
  void setPopupSlider(bool popup);

  ///
  /// Return the popup if ctkSliderWidget hasPopupSlider() is true, 0 otherwise.
  /// It can be useful to control where the popup shows up relative to the
  /// spinbox the popup \sa ctkPopupWidget::baseWidget.
  ctkPopupWidget* popup()const;

  ///
  /// Returns the spinbox synchronized with the slider. Be careful
  /// with what you do with the spinbox as the slider might change
  /// properties automatically.
  ctkDoubleSpinBox* spinBox();

  ///
  /// Returns the slider synchronized with the spinbox. Be careful
  /// with what you do with the slider as the spinbox might change
  /// properties automatically.
  ctkDoubleSlider* slider();
public Q_SLOTS:
  /// 
  /// Reset the slider and spinbox to zero (value and position)
  void reset();
  void setValue(double value);
  void setSpinBoxVisible(bool);

  /// Sets how many decimals the spinbox uses for displaying and
  /// interpreting doubles.
  void setDecimals(int decimals);

Q_SIGNALS:
  /// When tracking is on (default), valueChanged is emitted when the
  /// user drags the slider.
  /// If tracking is off, valueChanged() is emitted only when the user
  /// releases the mouse.
  /// \sa valueIsChanging QAbstractSlider::valueChanged
  void valueChanged(double value);

  /// valueIsChanging() is emitted whenever the slider is dragged and tracking
  /// is turned off. You might want to use valueChanged instead.
  /// It behaves the same way than QAbstractSlider::sliderMoved()
  /// \sa valueChanged QAbstractSlider::sliderMoved
  void valueIsChanging(double value);

protected Q_SLOTS:
  
  void startChanging();
  void stopChanging();
  void changeValue(double value);

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);
  
protected:
  QScopedPointer<ctkSliderWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSliderWidget);
  Q_DISABLE_COPY(ctkSliderWidget);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkSliderWidget::SynchronizeSiblings);

#endif
