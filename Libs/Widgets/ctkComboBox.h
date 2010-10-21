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

#ifndef __ctkComboBox_h
#define __ctkComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkComboBoxPrivate;

/// ctkComboBox is an advanced QComboBox. It allows the display of a default
/// text/icon when the combobox current index is invalid (-1). A typical
/// default text would be "Select a XXX..."
/// forceDefault can force the display of the default text at all time (with
/// a valid current index). The text displayed in the combo box can be
/// elided when the size is too small.
/// ctkComboBox works exactly the same as QComboBox by default.
class CTK_WIDGETS_EXPORT ctkComboBox : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
  Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
  Q_PROPERTY(bool forceDefault READ isDefaultForced WRITE forceDefault)
  Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)

public:
  /// Constructor, build a ctkComboBox that behave like QComboBox.
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

  /// Reimplemented for internal reasons
  virtual QSize minimumSizeHint()const;
  /// Reimplemented for internal reasons
  virtual QSize sizeHint()const;

protected:
  /// Reimplemented for internal reasons
  virtual void paintEvent(QPaintEvent*);

protected:
  QScopedPointer<ctkComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkComboBox);
  Q_DISABLE_COPY(ctkComboBox);
};

#endif
