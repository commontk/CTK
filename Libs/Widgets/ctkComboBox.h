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

#ifndef __ctkComboBox_h
#define __ctkComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkComboBoxPrivate;

/// \ingroup Widgets
/// \brief ctkComboBox is an advanced QComboBox.
/// It adds multiple features:
///  * Display a default text and/or icon when the combobox current index is
///    invalid (-1). A typical default text would be "Select a XXX...".
///    forceDefault can force the display of the default text at all time (with
///    a valid current index). The text displayed in the combo box can be
///    elided when the size is too small.
///  * Optionally prevent the mouse scroll events from changing the current
///    index.
/// ctkComboBox works exactly the same as QComboBox by default.
/// \sa QComboBox
class CTK_WIDGETS_EXPORT ctkComboBox : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
  Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
  Q_PROPERTY(bool forceDefault READ isDefaultForced WRITE forceDefault)
  Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)
  /// This property controls the behavior of the mouse scroll wheel.
  /// ScrollOn by default.
  /// /sa scrollWheelEffect, setScrollWheelEffect
  Q_PROPERTY(ScrollEffect scrollWheelEffect READ scrollWheelEffect WRITE setScrollWheelEffect)
  /// Current item's user data as string (Qt::UserRole role)
  Q_PROPERTY(QString currentUserDataAsString READ currentUserDataAsString WRITE setCurrentUserDataAsString)

  Q_ENUMS(ScrollEffect);
public:
  /// Constructor, build a ctkComboBox that behaves like QComboBox.
  explicit ctkComboBox(QWidget* parent = 0);
  virtual ~ctkComboBox();

  /// Empty by default (same behavior as QComboBox)
  void setDefaultText(const QString&);
  QString defaultText()const;

  /// Empty by default (same behavior as QComboBox)
  void setDefaultIcon(const QIcon&);
  QIcon defaultIcon()const;

  /// Force the display of the text/icon at all time (not only when the 
  /// current index is invalid). False by default.
  void forceDefault(bool forceDefault);
  bool isDefaultForced()const;

  /// setElideMode can elide the text displayed on the combobox.
  /// Qt::ElideNone by default (same behavior as QComboBox)
  void setElideMode(const Qt::TextElideMode& newMode);
  Qt::TextElideMode elideMode()const;

  /// \tbd turn into flags ?
  enum ScrollEffect
  {
    /// Scrolling is not possible with the mouse wheel.
    NeverScroll,
    /// Scrolling is always possible with the mouse wheel.
    AlwaysScroll,
    /// Scrolling is only possible if the combobox has the focus.
    /// The focus policy is automatically set to Qt::StrongFocus
    ScrollWithFocus,
    /// Scrolling is not possible when the combobox is inside a scroll area with
    /// a visible vertical scrollbar.
    ScrollWithNoVScrollBar
  };
  /// Return the scrollWheelEffect property value.
  /// \sa scrollEffect
  ScrollEffect scrollWheelEffect()const;
  /// Set the scrollWheelEffect property value.
  /// \sa scrollEffect
  void setScrollWheelEffect(ScrollEffect scroll);

  /// Reimplemented for internal reasons
  virtual QSize minimumSizeHint()const;
  /// Reimplemented for internal reasons
  virtual QSize sizeHint()const;

  /// Get current item's user data as string
  QString currentUserDataAsString()const;

public slots:
  /// Set current item based on user data
  void setCurrentUserDataAsString(QString userData);

protected:
  /// Reimplemented for internal reasons
  virtual void paintEvent(QPaintEvent* event);
  virtual void changeEvent(QEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

protected:
  QScopedPointer<ctkComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkComboBox);
  Q_DISABLE_COPY(ctkComboBox);
};

#endif
