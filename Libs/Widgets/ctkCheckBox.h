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

#ifndef __ctkCheckBox_h
#define __ctkCheckBox_h

// QT includes
#include <QCheckBox>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkCheckBoxPrivate;

/// \ingroup Widgets
/// ctkCheckBox is an advanced QCheckBox that gives more control
/// over its look and feel.
/// We can change the indicator check box by a new QIcon, with two mode On/Off.
/// The indicator icon size can be controled. see setIndicatorIconSize()

class CTK_WIDGETS_EXPORT ctkCheckBox : public QCheckBox
{
  Q_OBJECT
  Q_PROPERTY(QIcon indicatorIcon READ indicatorIcon WRITE setIndicatorIcon)
  Q_PROPERTY(QSize indicatorIconSize READ indicatorIconSize WRITE setIndicatorIconSize)

public:
  typedef QCheckBox Superclass;

  ctkCheckBox(QWidget *_parent = 0);
  virtual ~ctkCheckBox();

  void setIndicatorIcon(const QIcon& newIcon);
  QIcon indicatorIcon() const;

  /// Resize the indicator icon to Qsize.
  /// If newSize is bigger than the indicator icon's maximum size,
  /// The icon will get the icon's maximum size and not newSize.
  void setIndicatorIconSize(const QSize& newSize);
  QSize indicatorIconSize() const;

protected:
  QScopedPointer<ctkCheckBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCheckBox);
  Q_DISABLE_COPY(ctkCheckBox);
};

#endif
