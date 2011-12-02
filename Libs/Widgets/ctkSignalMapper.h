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

#ifndef __ctkSignalMapper_h
#define __ctkSignalMapper_h

// Qt includes
#include <QSignalMapper>
class QAction;

// CTKWidgets includes
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
/// Advanced QSignalMapper to simplify the use of mapping.
class CTK_WIDGETS_EXPORT ctkSignalMapper: public QSignalMapper
{
  Q_OBJECT
  
public:
  ctkSignalMapper(QObject* newParent = 0);
  
public Q_SLOTS:
  /// ctkSignalMapper exposes the map(QAction*) slot to be conveniently
  /// connected with signals that have a QAction* as their first argument.
  /// ctkSignalMapper reveals to be useful when connecting a
  /// QMenu::triggered(QAction*) or QActionGroup::triggered(QAction*).
  /// Example:
  /// <code>
  /// QMenu menu;
  /// QAction* action1 = menu.addAction("item1");
  /// QAction* action2 = menu.addAction("item2");
  /// QAction* action3 = menu.addAction("item3");
  /// ctkSignalMapper signalMapper;
  /// signalMapper.setMapping(action1, 1);
  /// signalMapper.setMapping(action2, 2);
  /// signalMapper.setMapping(action3, 3);
  /// QObject::connect(&menu, SIGNAL(triggered(QAction*)),
  ///                  &signalMapper, SLOT(map(QAction*)));
  /// //Connect the signal mapper mapped(int) signal with a listener
  /// QObject::connect(&signalMapper, SIGNAL(mapped(int)),
  ///                  &myObj, SLOT(actionTriggered(int)));
  /// // myObj::actionTriggered(2) will be called when the 2nd menu item will
  /// // be chosen in the menu.
  /// </code>
  void map(QAction* sender);

protected:
  Q_DISABLE_COPY(ctkSignalMapper);
};

#endif
