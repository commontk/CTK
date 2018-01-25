/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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
#include <QWidget>
#include <QHBoxLayout>
#include <QTimer>
#include <QToolButton>

// CTK includes
#include "ctkPathListWidget.h"
#include "ctkPathListButtonsWidget.h"
#include "ctkUtils.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkPathListWidgetWithButtonsTester : public QObject
{
  Q_OBJECT

private slots:

  void testButtons();

};

// ----------------------------------------------------------------------------
void ctkPathListWidgetWithButtonsTester::testButtons()
{
  QWidget topLevel;
  topLevel.setLayout(new QHBoxLayout());

  ctkPathListWidget pathList(&topLevel);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(1);
  pathList.setSizePolicy(sizePolicy);

  ctkPathListButtonsWidget pathListButtons(&topLevel);
  pathListButtons.init(&pathList);
  pathListButtons.setOrientation(Qt::Vertical);


  topLevel.layout()->addWidget(&pathList);
  topLevel.layout()->addWidget(&pathListButtons);

  topLevel.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&topLevel);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&topLevel);
#endif

  struct CloseModalDialog : public QRunnable
  {
    void run()
    {
      QTest::qWait(1000);
      QTimer::singleShot(0, QApplication::activeModalWidget(), SLOT(accept()));
      QTest::qWait(1000);
    }
  };
  QThreadPool::globalInstance()->start(new CloseModalDialog);
  QTest::mouseClick(pathListButtons.buttonAddDirectory(), Qt::LeftButton);

  QCOMPARE(pathList.count(), 1);
  QVERIFY(!pathListButtons.buttonRemove()->isEnabled());
  QVERIFY(!pathListButtons.buttonEdit()->isEnabled());

  pathList.selectAll();

  QVERIFY(pathListButtons.buttonRemove()->isEnabled());
  QVERIFY(pathListButtons.buttonEdit()->isEnabled());

  QTest::mouseClick(pathListButtons.buttonRemove(), Qt::LeftButton);
  QCOMPARE(pathList.count(), 0);
}


// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkPathListWidgetWithButtonsTest)
#include "moc_ctkPathListWidgetWithButtonsTest.cpp"
