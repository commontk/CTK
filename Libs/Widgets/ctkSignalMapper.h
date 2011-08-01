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

/// Advanced QSignalMapper to simplify the use of mapping.
class CTK_WIDGETS_EXPORT ctkSignalMapper: public QSignalMapper
{
  Q_OBJECT
  
public:
  ctkSignalMapper(QObject* newParent = 0);
  
public slots:
  /// ctkSignalMapper exposes the map(QAction*) slot to be conveniently
  /// connected with signals that have a QAction* as their first argument.
  /// ctkActionSignalMapper reveals to be useful when connecting a
  /// QActionGroup::triggered(QAction*).
  void map(QAction* sender);

protected:
  Q_DISABLE_COPY(ctkSignalMapper);
};

#endif
