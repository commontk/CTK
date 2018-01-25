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
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QTimer>

#if (QT_VERSION < 0x50000)
#include <QCleanlooksStyle>
#endif

// CTK includes
#include "ctkPushButton.h"
#include "ctkTest.h"

//-----------------------------------------------------------------------------
class ctkPushButtonTester : public QObject
{
  Q_OBJECT

private slots:

  void testDefaults();
};

// ----------------------------------------------------------------------------
void ctkPushButtonTester::testDefaults()
{
#if (QT_VERSION < 0x50000)
  QApplication::setStyle( new QCleanlooksStyle );
#endif

  ctkPushButton button("text");

  QCOMPARE(button.buttonTextAlignment(), Qt::AlignHCenter|Qt::AlignVCenter);
  QCOMPARE(button.iconAlignment(), Qt::AlignLeft|Qt::AlignVCenter);

  button.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&button);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&button);
#endif
  //qApp->exec();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkPushButtonTest)
#include "moc_ctkPushButtonTest.cpp"
