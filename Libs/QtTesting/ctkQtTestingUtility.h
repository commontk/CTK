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

#ifndef __ctkQtTestingUtility_h
#define __ctkQtTestingUtility_h

// QtTesting includes
#include <pqTestUtility.h>

// CTKQtTesting includes
#if !defined(NO_SYMBOL_EXPORT)
# include "ctkQtTestingExport.h"
#else
# define CTK_QTTESTING_EXPORT
#endif


class CTK_QTTESTING_EXPORT ctkQtTestingUtility : public pqTestUtility
{
  Q_OBJECT

public:
  typedef pqTestUtility Superclass;

  ctkQtTestingUtility(QObject* parent=0);
  ~ctkQtTestingUtility();

  void addDefaultCTKWidgetEventTranslatorsToTranslator(pqTestUtility* util);
  void addDefaultCTKWidgetEventPlayersToPlayer(pqTestUtility* util);

  void addTranslator(pqWidgetEventTranslator* translator);
  void addPlayer(pqWidgetEventPlayer* player);
};

#endif
