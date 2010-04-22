/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
/// The ctkButtonGroup class is a QButtonGroup that has a slightly
/// different behavior when exclusive. By default, a ctkButtonGroup is
/// exclusive. An exclusive ctkButtonGroup switches off the button that
///  was clicked if it was already checked.
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
