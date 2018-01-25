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

// CTK includes
#include "ctkSearchBox.h"
#include "ctkTest.h"

//-----------------------------------------------------------------------------
class ctkSearchBoxTester : public QObject
{
  Q_OBJECT

private slots:

  void testSignals();
};

// ----------------------------------------------------------------------------
void ctkSearchBoxTester::testSignals()
{
  ctkSearchBox searchBox;
  searchBox.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&searchBox);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&searchBox);
#endif

  QSignalSpy textEditedSpy(&searchBox, SIGNAL(textEdited(QString)));
  QSignalSpy textChangedSpy(&searchBox, SIGNAL(textChanged(QString)));

  searchBox.setText("abc");

  QCOMPARE(textEditedSpy.count(), 0);
  QCOMPARE(textChangedSpy.count(), 1);
  textEditedSpy.clear();
  textChangedSpy.clear();

  QTest::keyPress(&searchBox, 'd');
  QCOMPARE(textEditedSpy.count(), 1);
  QCOMPARE(textChangedSpy.count(), 1);
  textEditedSpy.clear();
  textChangedSpy.clear();

  QPoint clearIconCenter( searchBox.width() - 15, searchBox.height() / 2 );
  QTest::mouseClick(&searchBox, Qt::LeftButton, Qt::NoModifier, clearIconCenter);

  QCOMPARE(searchBox.text(), QString());
  QCOMPARE(textEditedSpy.count(), 1);
  QCOMPARE(textChangedSpy.count(), 1);
  textEditedSpy.clear();
  textChangedSpy.clear();

  //qApp->exec();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkSearchBoxTest)
#include "moc_ctkSearchBoxTest.cpp"
