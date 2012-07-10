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
#include <QVBoxLayout>

// CTK includes
#include "ctkFontButton.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>

// ----------------------------------------------------------------------------
class ctkFontButtonTester: public QObject
{
  Q_OBJECT
private slots:

  void testDefaults();
  void testDefaultFont();

  void testSetFont();
  void testSetFont_data();

  void testSetFontTextFormat();
  void testSetFontTextFormat_data();

  void testUpdateText();
  void testBrowseFont();
};

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testDefaults()
{
  ctkFontButton fontButton;
  QCOMPARE(fontButton.currentFont(), qApp->font());
  QCOMPARE(fontButton.fontTextFormat(), QString("fff-sss"));
}

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testDefaultFont()
{
  QWidget parentWidget;
  parentWidget.setFont(QFont("Helvetica", 6));
  ctkFontButton fontButton(&parentWidget);

  QCOMPARE(fontButton.currentFont(), qApp->font());

  fontButton.setFont(QFont("Arial", 9));
  QCOMPARE(fontButton.currentFont(), qApp->font());
}

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testSetFont()
{
  ctkFontButton fontButton;
  fontButton.setCurrentFont(QFont("Helvetica", 8));

  QSignalSpy spyFontChanged(&fontButton, SIGNAL(currentFontChanged(QFont)));

  QFETCH(QFont, currentFont);
  fontButton.setCurrentFont(currentFont);

  QFETCH(QFont, expectedCurrentFont);
  QFETCH(int, expectedFontChangedSignalCount);

  QCOMPARE(fontButton.currentFont(), expectedCurrentFont);
  QCOMPARE(spyFontChanged.count(), expectedFontChangedSignalCount);
}

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testSetFont_data()
{
  QTest::addColumn<QFont>("currentFont");
  QTest::addColumn<QFont>("expectedCurrentFont");
  QTest::addColumn<int>("expectedFontChangedSignalCount");

  QTest::newRow("invalid") << QFont() << QFont() << 1;
  QTest::newRow("new family") << QFont("Arial", 8) << QFont("Arial", 8) << 1;
  QTest::newRow("new size") << QFont("Helvetica", 12) << QFont("Helvetica", 12) << 1;
  QTest::newRow("new weight") << QFont("Helvetica", 8, QFont::Bold) << QFont("Helvetica", 8, QFont::Bold) << 1;
  QTest::newRow("new family+size+weight") << QFont("Arial", 12, QFont::Bold) << QFont("Arial", 12, QFont::Bold) << 1;
}

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testSetFontTextFormat()
{
  ctkFontButton fontButton;
  fontButton.setCurrentFont(QFont("Helvetica", 8, QFont::Normal, true));

  QFETCH(QString, fontTextFormat);
  fontButton.setFontTextFormat(fontTextFormat);
  QCOMPARE(fontButton.fontTextFormat(), fontTextFormat);

  QFETCH(QString, expectedText);
  QCOMPARE(fontButton.text(), expectedText);
}

// ----------------------------------------------------------------------------
void ctkFontButtonTester::testSetFontTextFormat_data()
{
  QTest::addColumn<QString>("fontTextFormat");
  QTest::addColumn<QString>("expectedText");

  QTest::newRow("null") << QString() << QString("");
  QTest::newRow("empty") << QString("") << QString("");
  QTest::newRow("fff") << QString("fff") << QString("Helvetica");
  QTest::newRow("ffffff") << QString("ffffff") << QString("HelveticaHelvetica");
  QTest::newRow("fff-ss-sss-ww-www-bb-bbb-ii-iii-uu-uuu-biu")
    << QString("fff-ss-sss-ww-www-bb-bbb-ii-iii-uu-uuu-biu")
    << QString("Helvetica-8-8pt-50-Normal---i-italic----i-");
  QTest::newRow("biuu") << QString("biuu") << QString("-i-u");
}

//-----------------------------------------------------------------------------
void ctkFontButtonTester::testUpdateText()
{
  ctkFontButton fontButton(QFont("Helvetica", 8));
  QCOMPARE(fontButton.text(), QString("Helvetica-8pt"));

  fontButton.setText("Custom text");
  QCOMPARE(fontButton.text(), QString("Custom text"));

  fontButton.setCurrentFont(QFont("Helvetica", 9));
  QCOMPARE(fontButton.text(), QString("Helvetica-9pt"));

  fontButton.setText("Custom text");
  QCOMPARE(fontButton.text(), QString("Custom text"));

  fontButton.setFontTextFormat("fff-ss");
  QCOMPARE(fontButton.text(), QString("Helvetica-9"));
}

//-----------------------------------------------------------------------------
void ctkFontButtonTester::testBrowseFont()
{
  ctkFontButton fontButton;
  QTimer::singleShot(200, qApp, SLOT(quit()));
  QTimer::singleShot(100, &fontButton, SLOT(browseFont()));
  fontButton.show();
  qApp->exec();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkFontButtonTest)
#include "moc_ctkFontButtonTest.cpp"
