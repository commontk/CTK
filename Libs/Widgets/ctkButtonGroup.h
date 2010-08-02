/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkButtonGroup_h
#define __ctkButtonGroup_h

// Qt includes
#include <QButtonGroup>

// CTK includes
#include <ctkPimpl.h>

#include "CTKWidgetsExport.h"

class ctkButtonGroupPrivate;

///
/// ctkButtonGroup is a QButtonGroup with a different behavior when exclusive.
/// An exclusive ctkButtonGroup switches off the previously checked button when 
/// a new button is checked. ctkButtonGroup doesn't enforce that 1 button is
/// checked at all time (contrary to QButtonGroup). If a button is checked it is
/// possible to uncheck it without having to check another button.
/// Use ctkButtonGroup the same way than QButtonGroup (see QButtonGroup doc).
/// i.e. : ctkButtonGroup* buttonGroup = new ctkButtonGroup(parent);
/// buttonGroup->addButton(button1);
/// buttonGroup->addButton(button2);
/// By default ctkButtonGroup is exclusive. 
class CTK_WIDGETS_EXPORT ctkButtonGroup : public QButtonGroup
{
  Q_OBJECT
public:
  explicit ctkButtonGroup(QObject *_parent = 0);

protected slots:
  void onButtonClicked(QAbstractButton * button);
  void onButtonPressed(QAbstractButton * button);

private:
  CTK_DECLARE_PRIVATE(ctkButtonGroup);
};

#endif
