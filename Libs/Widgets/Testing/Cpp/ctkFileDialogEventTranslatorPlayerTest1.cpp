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
#include <QCheckBox>
#include <QDebug>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkFileDialog.h"
#include "ctkFileDialogEventPlayer.h"
#include "ctkFileDialogEventTranslator.h"
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
  ctkFileDialog* widget = reinterpret_cast<ctkFileDialog*>(data);
  QStringList expectedSelectedFiles;
  expectedSelectedFiles << CTK_SOURCE_DIR"/ctkLogo.png";
  expectedSelectedFiles << CTK_SOURCE_DIR"/ctkLogo-small.png";
  expectedSelectedFiles.sort();

  QStringList selectedFiles = widget->selectedFiles();
  selectedFiles.sort();

  CTKCOMPARE(selectedFiles, expectedSelectedFiles);
}

//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
{
  ctkFileDialog* widget = reinterpret_cast<ctkFileDialog*>(data);
  QStringList actual =  widget->selectedFiles();
  QString expected = CTK_SOURCE_DIR"/ctkLogo.png";

  if( actual.count() == 1)
    {
    CTKCOMPARE(actual[0], expected);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
}
//-----------------------------------------------------------------------------
void checkFinalWidgetState3(void* data)
{
  ctkFileDialog* widget = reinterpret_cast<ctkFileDialog*>(data);
  CTKCOMPARE(widget->isHidden(), true);
}
}

//-----------------------------------------------------------------------------
int ctkFileDialogEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  testUtility->addDataDirectory("CTK_SOURCE_DIR", QDir(CTK_SOURCE_DIR));
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkFileDialogEventPlayer(etpWidget.testUtility()));
  etpWidget.addWidgetEventTranslator(new ctkFileDialogEventTranslator(etpWidget.testUtility()));

  // Test case 1
  ctkFileDialog* fileDialog = new ctkFileDialog() << ctkSetName("fileDialog1");
  fileDialog->setFileMode(QFileDialog::ExistingFiles);
  fileDialog->setNameFilter("Images (*.png *.xpm *.jpg)");
  fileDialog->setViewMode(QFileDialog::Detail);
  QCheckBox* checkBox = new QCheckBox << ctkSetName("checkBox1");
  fileDialog->setBottomWidget(checkBox, "Foo Bar:");
  QObject::connect(checkBox, SIGNAL(toggled(bool)),
                   fileDialog, SLOT(setAcceptButtonEnable(bool)));
  fileDialog->setAcceptButtonEnable(false);

  etpWidget.addTestCase(fileDialog,
                        xmlDirectory + "ctkFileDialogEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkFileDialog* fileDialog2 = new ctkFileDialog() << ctkSetName("fileDialog2");
  fileDialog2->setFileMode(QFileDialog::AnyFile);
  fileDialog2->setViewMode(QFileDialog::Detail);
  QCheckBox* checkBox2 = new QCheckBox << ctkSetName("checkBox2");
  fileDialog2->setBottomWidget(checkBox2, "Foo Bar:");
  QObject::connect(checkBox2, SIGNAL(toggled(bool)),
                   fileDialog2, SLOT(setAcceptButtonEnable(bool)));
  fileDialog2->setAcceptButtonEnable(false);

  etpWidget.addTestCase(fileDialog2,
                        xmlDirectory + "ctkFileDialogEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // Test case 2
  ctkFileDialog* fileDialog3 = new ctkFileDialog() << ctkSetName("fileDialog3");
  fileDialog3->setFileMode(QFileDialog::AnyFile);
  fileDialog3->setViewMode(QFileDialog::Detail);
  QCheckBox* checkBox3 = new QCheckBox << ctkSetName("checkBox3");
  fileDialog3->setBottomWidget(checkBox3, "Foo Bar:");
  QObject::connect(checkBox3, SIGNAL(toggled(bool)),
                   fileDialog3, SLOT(setAcceptButtonEnable(bool)));
  fileDialog3->setAcceptButtonEnable(false);

  etpWidget.addTestCase(fileDialog3,
                        xmlDirectory + "ctkFileDialogEventTranslatorPlayerTest3.xml",
                        &checkFinalWidgetState3);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();

}

