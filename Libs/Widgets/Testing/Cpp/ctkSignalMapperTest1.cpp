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

// Qt includes
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QActionGroup>
#include <QSignalSpy>

// CTK includes
#include "ctkSignalMapper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSignalMapperTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QAction action1(0);
  QAction action2(0);
  QAction action3(0);

  QActionGroup actionGroup(0);
  actionGroup.setExclusive(true);
  actionGroup.addAction(&action1);
  actionGroup.addAction(&action2);
  actionGroup.addAction(&action3);

  ctkSignalMapper signalMapper;
  signalMapper.setMapping(&action1, 1);
  signalMapper.setMapping(&action2, 2);
  signalMapper.setMapping(&action3, 3);

  QObject::connect(&actionGroup, SIGNAL(triggered(QAction*)),
                   &signalMapper, SLOT(map(QAction*)));
  QSignalSpy signalSpy(&signalMapper, SIGNAL(mapped(int)));

  action2.trigger();

  if (signalSpy.count() != 1 ||
      signalSpy.at(0).at(0).toInt() != 2)
    {
    std::cerr << "ctkSignalMapper::map(QAction*) failed." << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
