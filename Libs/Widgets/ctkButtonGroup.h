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

#ifndef __ctkButtonGroup_h
#define __ctkButtonGroup_h

// Qt includes
#include <QButtonGroup>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkButtonGroupPrivate;

/// \ingroup Widgets
///
/// ctkButtonGroup is a QButtonGroup with a different behavior when exclusive.
/// An exclusive ctkButtonGroup switches off the previously checked button when
/// a new button is checked. ctkButtonGroup doesn't enforce that 1 button is
/// checked at all time (contrary to QButtonGroup). If a button is checked it is
/// possible to uncheck it without having to check another button.
/// Use ctkButtonGroup the same way than QButtonGroup.
/// \code
/// ctkButtonGroup* buttonGroup = new ctkButtonGroup(parent);
/// buttonGroup->addButton(button1);
/// buttonGroup->addButton(button2);
/// \endcode
/// By default ctkButtonGroup is exclusive.
/// \sa QButtonGroup
class CTK_WIDGETS_EXPORT ctkButtonGroup : public QButtonGroup
{
  Q_OBJECT
public:
  explicit ctkButtonGroup(QObject *_parent = 0);
  virtual ~ctkButtonGroup();

public Q_SLOTS:
  /// Check or uncheck the button.
  void setChecked(QAbstractButton* button, bool checked = true);

protected Q_SLOTS:
  void onButtonClicked(int button);
  void onButtonPressed(int button);

protected:
  QScopedPointer<ctkButtonGroupPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkButtonGroup);
  Q_DISABLE_COPY(ctkButtonGroup);
};

#endif
