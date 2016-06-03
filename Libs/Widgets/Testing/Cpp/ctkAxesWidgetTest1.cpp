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
#include <QStringList>
#include <QTimer>

// CTK includes
#include "ctkAxesWidget.h"
#include "ctkCoreTestingMacros.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAxesWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAxesWidget axes(0);
  if (axes.currentAxis() != ctkAxesWidget::None)
    {
    std::cerr << "ctkAxesWidget default axis is wrong: "
              << static_cast<int>(axes.currentAxis()) << std::endl;
    return EXIT_FAILURE;
    }
  
  QSignalSpy spy(&axes, SIGNAL(currentAxisChanged(ctkAxesWidget::Axis)));
  axes.setCurrentAxis(ctkAxesWidget::Anterior);

  if (axes.currentAxis() != ctkAxesWidget::Anterior ||
      spy.count() != 1)
    {
    std::cerr << "ctkAxesWidget default axis is wrong: "
              << static_cast<int>(axes.currentAxis()) << " " << spy.count() << std::endl;
    return EXIT_FAILURE;
    }
  if ( qvariant_cast<ctkAxesWidget::Axis>(spy.takeFirst().at(0)) != ctkAxesWidget::Anterior)
    {
    std::cerr << "ctkAxesWidget fired the wrong current axis : "
              << spy.takeFirst().at(0).toInt() << std::endl;
    return EXIT_FAILURE;
    }

  axes.setAutoReset(true);
  if ((axes.autoReset() != true) ||
      (axes.currentAxis() != ctkAxesWidget::None))
    {
    std::cerr << "ctkAxesWidget::setAutoReset failed: "
              << static_cast<int>(axes.currentAxis()) << std::endl;
    return EXIT_FAILURE;
    }
  spy.clear();
  axes.setCurrentAxis(ctkAxesWidget::Right);
  if (axes.currentAxis() != ctkAxesWidget::None ||
      spy.count() != 2)
    {
    std::cerr << "ctkAxesWidget::setCurrentAxis() with autoReset ON failed: "
              << static_cast<int>(axes.currentAxis()) << " " << spy.count() << std::endl;
    return EXIT_FAILURE;
    }
  if (qvariant_cast<ctkAxesWidget::Axis>(spy[0].at(0)) != ctkAxesWidget::Right ||
      qvariant_cast<ctkAxesWidget::Axis>(spy[1].at(0)) != ctkAxesWidget::None)
    {
    std::cerr << "ctkAxesWidget::setCurrentAxis() with autoReset ON failed: "
            << spy[0].at(0).toInt() << " " << spy[1].at(0).toInt() << std::endl;
    return EXIT_FAILURE;
    }


  // Test axesLabels/setAxesLabels
  CHECK_BOOL(axes.setAxesLabels(QStringList()), false);

  QStringList emptyAxesLabels =
      QStringList() << "" << "" << "" << "" << "" << "";
  CHECK_BOOL(axes.setAxesLabels(emptyAxesLabels), true);
  CHECK_QSTRINGLIST(axes.axesLabels(), emptyAxesLabels);

  QStringList singleLetterAxesLabels =
      QStringList() << "W" << "E" << "S" << "N" << "Z" << "z";
  CHECK_BOOL(axes.setAxesLabels(singleLetterAxesLabels), true);
  CHECK_QSTRINGLIST(axes.axesLabels(), singleLetterAxesLabels);

  QStringList additionalAxesLabels = singleLetterAxesLabels;
  additionalAxesLabels.append("X");
  CHECK_BOOL(axes.setAxesLabels(additionalAxesLabels), true);
  CHECK_QSTRINGLIST(axes.axesLabels(), singleLetterAxesLabels);

  axes.setAutoReset(false);
  axes.setAutoReset(true);
  axes.setWindowTitle("AutoReset=On");
  axes.show();

  ctkAxesWidget axes2;
  axes2.setWindowTitle("AutoReset=Off");
  axes2.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

