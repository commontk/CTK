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
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkModalityWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkModalityWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkModalityWidget modalityWidget;
  
  QSignalSpy spy(&modalityWidget, SIGNAL(selectedModalitiesChanged(QStringList)));
  modalityWidget.unselectAll();

  if (spy.count() != 1)
    {
    std::cerr << "ctkModalityWidget::setProperty(): "
              << spy.count() << std::endl;
    return EXIT_FAILURE;
    }
  spy.clear();

  modalityWidget.selectAll();
  if (!modalityWidget.areAllModalitiesSelected())
    {
    std::cerr << "ctkModalityWidget::selectAll() failed" << std::endl;
    return EXIT_FAILURE;
    }

  modalityWidget.selectModality("CR", false);
  if (modalityWidget.areAllModalitiesSelected())
    {
    std::cerr << "ctkModalityWidget::selectModality() failed "
              << " when unselect \"CR\"" << std::endl;
    return EXIT_FAILURE;
    }

  modalityWidget.showAll();

  modalityWidget.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

