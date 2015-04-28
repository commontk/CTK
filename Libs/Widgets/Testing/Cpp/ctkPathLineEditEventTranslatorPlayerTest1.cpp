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
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkPathLineEdit.h"
#include "ctkPathLineEditEventPlayer.h"
#include "ctkPathLineEditEventTranslator.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkSetName.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkPathLineEdit* widget = reinterpret_cast<ctkPathLineEdit*>(data);
  Q_UNUSED(widget);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkPathLineEdit* widget = reinterpret_cast<ctkPathLineEdit*>(data);
  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int ctkPathLineEditEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkPathLineEditEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkPathLineEditEventTranslator);

  // Test case 1
  QWidget* widget = new QWidget(0);
  ctkPathLineEdit* button = new ctkPathLineEdit("Files",
                                                QStringList() << "Images (*.png *.jpg)" << "Text (*.txt)",
                                                ctkPathLineEdit::Files, widget) << ctkSetName("PathLineEdit1");
  button->setMinimumWidth(250);
  button->setCurrentPath(QDir::tempPath());

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkPathLineEditEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

    // Test case 2
  QWidget* widget2 = new QWidget(0);
  ctkPathLineEdit* button2 = new ctkPathLineEdit("Dirs", QStringList("CTK*"), ctkPathLineEdit::AllEntries, widget2) << ctkSetName("PathLineEdit2");
  button2->setMinimumWidth(250);

  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkPathLineEditEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);
  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

