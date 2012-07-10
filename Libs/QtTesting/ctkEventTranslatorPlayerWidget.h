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
#ifndef __ctkEventTranslatorPlayerWidget_h
#define __ctkEventTranslatorPlayerWidget_h

// Qt includes
#include <QDateTime>
#include <QMainWindow>

// CTK includes
#if !defined(NO_SYMBOL_EXPORT)
# include "ctkQtTestingExport.h"
#else
# define CTK_QTTESTING_EXPORT
#endif
class ctkCallback;
class ctkEventTranslatorPlayerWidgetPrivate;

// QtTesting includes
class pqTestUtility;
class pqWidgetEventPlayer;
class pqWidgetEventTranslator;

//-----------------------------------------------------------------------------
#define CTKCOMPARE(actual, expected) \
do \
{\
    if (!ctkEventTranslatorPlayerWidget::compare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
      { \
      return;\
      } \
} while (0)

//-----------------------------------------------------------------------------
class CTK_QTTESTING_EXPORT ctkEventTranslatorPlayerWidget
  : public QMainWindow
{
  Q_OBJECT
public:
  typedef QMainWindow Superclass;
  ctkEventTranslatorPlayerWidget();
  ~ctkEventTranslatorPlayerWidget();

  void addTestCase(QWidget* widget, QString fileName, void(*newCallback)(void* data));
  void addTestCase(QDialog* dialog, QString fileName, void(*newCallback)(void* data));

  void setTestUtility(pqTestUtility* newTestUtility);
  pqTestUtility* testUtility() const;

  void addWidgetEventPlayer(pqWidgetEventPlayer* player);
  void addWidgetEventTranslator(pqWidgetEventTranslator* translator);

  static const char* enumValueToKey(QObject* object, const char* enumName, int value);

  static bool compare(const double& actual, const double& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const int& actual, const int& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const QString& actual,const QString& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const QStringList& actual,const QStringList& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const QDateTime& actual,const QDateTime& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const QColor& actual,const QColor& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);
  static bool compare(const QImage& actual,const QImage& expected,
                      const char* actualName, const char* expectedName, const char * function, int line);

public slots:
  void play();

protected slots:
  void record(int currentTestCase);
  bool play(int currentTestCase);
  void popupDialog();
  void onClickedPlayback(bool);
  void onClickedRecord(bool);
  void switchTestCase(int testCase);

signals:
  void startPlayerBack(QWidget* widget);
  void playerDone(QWidget* widget);
  void recordDone(QWidget* widget);

protected:
  QScopedPointer< ctkEventTranslatorPlayerWidgetPrivate > d_ptr;
private:
  Q_DECLARE_PRIVATE(ctkEventTranslatorPlayerWidget);
  Q_DISABLE_COPY(ctkEventTranslatorPlayerWidget);

  struct InfoTestCase {
    QWidget*       Widget;
    ctkCallback*   Callback;
    QString        FileName;
    bool           Dialog;
  };

};

#endif
