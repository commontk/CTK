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

#ifndef __ctkPopupWidget_h
#define __ctkPopupWidget_h

// CTK includes
#include "ctkBasePopupWidget.h"

class ctkPopupWidgetPrivate;

/// \ingroup Widgets
/// ctkPopupWidget is a specialization of ctkBasePopupWidget that handles
/// the opening and closing of the popup.
/// Below is an example of a popup slider that opens and closes next to a
/// button
/// \code
/// ctkPopupWidget* popup = new ctkPopupWidget(pushButton);
/// popup->setAlignment(Qt::AlignRight | Qt::AlignTop | Qt::AlignBottom);
/// popup->setOrientation(Qt::Horizontal);
/// QHBoxLayout* popupLayout = new QHBoxLayout(popup);
/// QSlider* popupSlider = new QSlider(popup);
/// popupLayout->addWidget(popupSlider);
/// \endcode
/// \sa ctkBasePopupWidget
class CTK_WIDGETS_EXPORT ctkPopupWidget : public ctkBasePopupWidget
{
  Q_OBJECT

  /// Control whether the popup listens to the application and baseWidget
  /// events and decides if it needs to be permanently or temporarily hidden.
  /// You might want to setActive(false) when embedding the popup
  /// into a static layout instead of having it top-level (no parent).
  /// Consider also removing its windowFlags (Qt::ToolTip |
  /// Qt::FramelessWindowHint) and removing the baseWidget.
  /// True by default
  /// \sa isActive(), setActive()
  Q_PROPERTY( bool active READ isActive WRITE setActive)

  /// Control whether the popup automatically opens when the mouse
  /// enter the widget. True by default
  /// \sa autoShow(), setAutoShow()
  Q_PROPERTY( bool autoShow READ autoShow WRITE setAutoShow)

  /// Time in ms to wait before opening the popup if autoShow is set.
  /// 20ms by default
  /// \sa showDelay(), setShowDelay()
  Q_PROPERTY( int showDelay READ showDelay WRITE setShowDelay)

  /// Control whether the popup automatically closes when the mouse
  /// leaves the widget. True by default.
  /// \sa autoHide(), setAutoHide()
  Q_PROPERTY( bool autoHide READ autoHide WRITE setAutoHide)

  /// Time in ms to wait before closing the popup if autoHide is set.
  /// 200ms by default
  /// \sa hideDelay(), setHideDelay()
  Q_PROPERTY( int hideDelay READ hideDelay WRITE setHideDelay)

public:
  typedef ctkBasePopupWidget Superclass;
  /// By default, the parent is the \a baseWidget.
  /// \sa baseWidget()
  explicit ctkPopupWidget(QWidget* parent = 0);
  virtual ~ctkPopupWidget();

  bool isActive()const;
  void setActive(bool);

  bool autoShow()const;
  /// Calling setAutoShow automatically updates opens the popup if the cursor
  /// is above the popup or the base widget.
  void setAutoShow(bool);

  int showDelay()const;
  void setShowDelay(int delay);

  bool autoHide()const;
  /// Don't automatically close the popup when leaving the widget.
  /// Calling setAutoHide automatically updates the state close the popup
  /// if the mouse is not over the popup nor the base widget.
  void setAutoHide(bool autoHide);

  int hideDelay()const;
  void setHideDelay(int delay);

public Q_SLOTS:
  /// Convenient function that calls setAutoHide(!pin) and opens the popup
  /// if pin is true regardless of the value of \a AutoShow.
  /// It is typically connected with a checkable button to anchor the popup.
  void pinPopup(bool pin);

public:
  /// Reimplemented for internal reasons
  virtual void hidePopup();

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  using QEnterEvent = QEvent;
#endif

protected:
  virtual void leaveEvent(QEvent* event);
  virtual void enterEvent(QEnterEvent* event);
  virtual bool eventFilter(QObject* obj, QEvent* event);

  /// Widget the popup is attached to. It opens right under \a baseWidget
  /// and if the ctkPopupWidget sizepolicy contains the growFlag/shrinkFlag,
  /// it tries to resize itself to fit the same width of \a baseWidget.
  virtual void setBaseWidget(QWidget* baseWidget);

protected Q_SLOTS:
  void updatePopup();
  virtual void onEffectFinished();

private:
  Q_DECLARE_PRIVATE(ctkPopupWidget);
  Q_DISABLE_COPY(ctkPopupWidget);
};

#endif
