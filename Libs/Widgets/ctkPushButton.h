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

#ifndef __ctkPushButton_h
#define __ctkPushButton_h

// Qt includes
#include <QPushButton>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkPushButtonPrivate;

/// \ingroup Widgets
/// Description
/// ctkPushButton is an advanced QPushButton. It can control the alignment of text and icons.
class CTK_WIDGETS_EXPORT ctkPushButton : public QPushButton
{
  Q_OBJECT
  /// Set the alignment of the text on the button,
  /// Qt::AlignHCenter|Qt::AlignVCenter by default.
  /// \sa textAlignment(), setTextAlignment(), iconAlignment
  Q_PROPERTY(Qt::Alignment buttonTextAlignment READ buttonTextAlignment WRITE setButtonTextAlignment)
  /// Set the alignment of the icon with regard to the text.
  /// Qt::AlignLeft|Qt::AlignVCenter by default.
  /// \sa iconAlignment(), setIconAlignment(), textAlignment
  Q_PROPERTY(Qt::Alignment iconAlignment READ iconAlignment WRITE setIconAlignment)

public:
  ctkPushButton(QWidget *parent = 0);
  ctkPushButton(const QString& text, QWidget *parent = 0);
  ctkPushButton(const QIcon& icon, const QString& text, QWidget *parent = 0);
  virtual ~ctkPushButton();

  /// Set the buttonTextAlignment property value.
  /// \sa buttonTextAlignment
  void setButtonTextAlignment(Qt::Alignment buttonTextAlignment);
  /// Return the buttonTextAlignment property value.
  /// \sa buttonTextAlignment
  Qt::Alignment buttonTextAlignment()const;

  /// Set the iconAlignment property value.
  /// \sa iconAlignment
  void setIconAlignment(Qt::Alignment iconAlignment);
  /// Return the iconAlignment property value.
  /// \sa iconAlignment
  Qt::Alignment iconAlignment()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

protected:
  /// Reimplemented for internal reasons
  virtual void paintEvent(QPaintEvent*);

protected:
  QScopedPointer<ctkPushButtonPrivate> d_ptr;
  ctkPushButton(ctkPushButtonPrivate*, QWidget* parent = 0);

private:
  Q_DECLARE_PRIVATE(ctkPushButton);
  Q_DISABLE_COPY(ctkPushButton);
};

#endif
