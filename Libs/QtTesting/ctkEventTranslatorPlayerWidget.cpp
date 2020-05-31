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
// QT includes
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMetaEnum>
#include <QTextStream>
#include <QVBoxLayout>

// CTKTesting includes
#include "ctkCallback.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkQtTestingUtility.h"
#include "ctkXMLEventObserver.h"
#include "ctkXMLEventSource.h"
#include "ui_ctkEventTranslatorPlayerWidget.h"

// Standard include
#include <cmath>

//-----------------------------------------------------------------------------
class ctkEventTranslatorPlayerWidgetPrivate
  : public Ui_ctkEventTranslatorPlayerWidget
{
public:
  ~ctkEventTranslatorPlayerWidgetPrivate();
  QList<ctkEventTranslatorPlayerWidget::InfoTestCase*>  TestCase;
  pqTestUtility*        TestUtility;
};

ctkEventTranslatorPlayerWidgetPrivate::~ctkEventTranslatorPlayerWidgetPrivate()
{
  delete this->TestUtility;
  this->TestUtility = 0;
}

//-----------------------------------------------------------------------------
ctkEventTranslatorPlayerWidget::ctkEventTranslatorPlayerWidget()
  :  Superclass()
  , d_ptr(new ctkEventTranslatorPlayerWidgetPrivate)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->setupUi(this);

  QObject::connect(d->TranslatorButton, SIGNAL(clicked(bool)),
                   this, SLOT(onClickedRecord(bool)));
  QObject::connect(d->PlayerButton, SIGNAL(clicked(bool)),
                   this, SLOT(onClickedPlayback(bool)));
  QObject::connect(d->TestCaseComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(switchTestCase(int)));

  d->TestUtility = 0;
}

//-----------------------------------------------------------------------------
ctkEventTranslatorPlayerWidget::~ctkEventTranslatorPlayerWidget()
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->TestUtility = 0;
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::addTestCase(QWidget *widget,
                                                 QString fileName,
                                                 void (*newCallback)(void * data))
{
  Q_D(ctkEventTranslatorPlayerWidget);

  InfoTestCase* infoTestCase = new InfoTestCase;
  infoTestCase->Widget = widget;
  infoTestCase->FileName = fileName;
  infoTestCase->Callback = new ctkCallback(newCallback);
  infoTestCase->Callback->setCallbackData(widget);
  infoTestCase->Dialog = false;

  d->TestCase.push_back(infoTestCase);
  
  d->stackedWidget->addWidget(widget);
  d->TestCaseComboBox->addItem(QString::number(d->TestCase.count()),
                               QVariant(d->TestCase.count()));
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::addTestCase(QDialog *dialog,
                                                 QString fileName,
                                                 void (*newCallback)(void * data))
{
  Q_D(ctkEventTranslatorPlayerWidget);

  InfoTestCase* infoTestCase = new InfoTestCase;
  infoTestCase->Widget = dialog;
  infoTestCase->FileName = fileName;
  infoTestCase->Callback = new ctkCallback(newCallback);
  infoTestCase->Callback->setCallbackData(dialog);
  infoTestCase->Dialog = false;

  d->TestCase.push_back(infoTestCase);

//  QVBoxLayout* layout = new QVBoxLayout();
  QPushButton* button = new QPushButton("Open the Dialog");
  connect(button, SIGNAL(clicked(bool)), this, SLOT(popupDialog()));

  d->stackedWidget->addWidget(button);
  d->TestCaseComboBox->addItem(QString::number(d->TestCase.count()),
                               QVariant(d->TestCase.count()));
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::setTestUtility(pqTestUtility* newTestUtility)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->TestUtility = newTestUtility;
  d->TestUtility->addEventObserver("xml", new ctkXMLEventObserver(d->TestUtility));
  ctkXMLEventSource* eventSource = new ctkXMLEventSource(d->TestUtility);
  eventSource->setRestoreSettingsAuto(true);
  d->TestUtility->addEventSource("xml", eventSource);
}

//-----------------------------------------------------------------------------
pqTestUtility* ctkEventTranslatorPlayerWidget::testUtility() const
{
  Q_D(const ctkEventTranslatorPlayerWidget);
  return d->TestUtility;
}

//-----------------------------------------------------------------------------
void  ctkEventTranslatorPlayerWidget::addWidgetEventPlayer(
  pqWidgetEventPlayer* player)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->TestUtility->eventPlayer()->addWidgetEventPlayer(player);
}

//-----------------------------------------------------------------------------
void  ctkEventTranslatorPlayerWidget::addWidgetEventTranslator(
  pqWidgetEventTranslator* translator)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->TestUtility->eventTranslator()->addWidgetEventTranslator(translator);
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::record(int currentTestCase)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  if(d->TestCase.count() == 0 ||
     currentTestCase > d->TestCase.count() - 1)
    {
    qWarning() << "Problem with the test case. Please verify that you added a test case.";
    return;
    }

  // We load the xml and check if it is different form the other test case
  QFile* filexml = new QFile(d->TestCase[currentTestCase]->FileName);
  if (!filexml->open(QIODevice::ReadWrite))
    {
    qWarning() << "The file .xml was not created";
    return;
    }
  for(int i = 0 ; i < currentTestCase && i != currentTestCase; i++)
    {
    if (d->TestCase[i]->FileName ==
          d->TestCase[currentTestCase]->FileName)
      {
      qWarning() << "This xml file should already recorded\n";
      return;
      }
    }

  d->TestUtility->recordTests(filexml->fileName());
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::play(int currentTestCase)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  if(d->TestCase.count() == 0)
    {
    qWarning() << "No test case had been added. Please add a test case.";
    return false;
    }

  // Connect the slot player done to the the good callback
  QObject::connect(this, SIGNAL(playerDone(QWidget*)),
                   d->TestCase[currentTestCase]->Callback, SLOT(invoke()));

  if (!QFile::exists(d->TestCase[currentTestCase]->FileName))
    {
    qWarning() << "No .xml file for this test case";
    return false;
    }

  if (!d->TestUtility->playTests(QStringList(d->TestCase[currentTestCase]->FileName)))
    {
    qWarning() << "The Test case " << currentTestCase
               << " playback has failed !";
    return false;
    }
  emit this->playerDone(d->TestCase[currentTestCase]->Widget);

  QObject::disconnect(d->TestCase[currentTestCase]->Callback);
  return true;
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::play()
{
  Q_D(ctkEventTranslatorPlayerWidget);
  bool success = true;
  for(int i = 0 ; i < d->TestCase.count() ; i++ )
    {
    d->TestCaseComboBox->setCurrentIndex(i);
    success &= this->play(i);
    }
  QApplication::exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::popupDialog()
{
  Q_D(ctkEventTranslatorPlayerWidget);
  QDialog* widget = qobject_cast<QDialog*>(d->TestCase[d->TestCaseComboBox->currentIndex()]->Widget);
  widget->exec();
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::onClickedPlayback(bool)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  this->play(d->TestCaseComboBox->currentIndex());
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::onClickedRecord(bool)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  this->record(d->TestCaseComboBox->currentIndex());
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::switchTestCase(int index)
{
  Q_D(ctkEventTranslatorPlayerWidget);
  d->stackedWidget->setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
const char* enumValueToKey(QObject* object, const char* enumName, int value)
{
  const QMetaObject * metaObject = object->metaObject();
  QMetaEnum theEnum = metaObject->enumerator(metaObject->indexOfEnumerator(enumName));
  return theEnum.valueToKey(value);
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const double &actual,
                                             const double& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const int &actual,
                                             const int& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QString& actual,
                                             const QString& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QStringList& actual,
                                             const QStringList& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.join(" ") << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.join(" ") << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.join(" ") << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.join(" ") << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QDateTime& actual,
                                             const QDateTime& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.date().toString() << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.date().toString() << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.date().toString() << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.date().toString() << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QColor& actual,
                                             const QColor& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = R:" << actual.red() << " G:"<< actual.green() << " B:" << actual.blue() << "\n"
        << "\tExpected value : '" << expectedName << "' = R:" << expected.red() << " G:"<< expected.green() << " B:" << expected.blue()<< Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = R:" << actual.red() << " G:"<< actual.green() << " B:" << actual.blue() << "\n"
        << "\tExpected value : '" << expectedName << "' = R:" << expected.red() << " G:"<< expected.green() << " B:" << expected.blue()<< endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QImage& actual,
                                             const QImage& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  double totaldiff = 0.0 ; //holds the number of different pixels

  // images are considered the same if both contain a null image
  if (actual.isNull() && expected.isNull())
    {
    return true;
    }
  // images are not the same if one images contains a null image
  if (actual.isNull() || expected.isNull())
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - 1 image is Null " << function << "\n" << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - 1 image is Null " << function << "\n" << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  // images do not have the same size
  if (actual.size() != expected.size())
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images don't have the same size " << function << "\n"
        << "\tActual value : '" << actualName << "' = W:" << actual.width() << " H:"<< actual.height() << "\n"
        << "\tExpected value : '" << expectedName << "' = W:" << expected.width() << " H:"<< expected.height() << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images don't have the same size " << function << "\n"
        << "\tActual value : '" << actualName << "' = W:" << actual.width() << " H:"<< actual.height() << "\n"
        << "\tExpected value : '" << expectedName << "' = W:" << expected.width() << " H:"<< expected.height() << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }

  QImage a = actual.convertToFormat(QImage::Format_ARGB32);
  QImage e = expected.convertToFormat(QImage::Format_ARGB32);

  for ( int y=0; y<a.height(); y++ )
    {
    for ( int x=0; x<a.width(); x++ )
      {
      QRgb actPix = a.pixel(x, y);
      QRgb expPix = e.pixel(x, y);
      if (qAlpha(actPix) == 0 && qAlpha(expPix) == 0)
        {
        continue;
        }
      if (actPix != expPix)
        {
        totaldiff ++;
        }
      }
    }
  totaldiff = (totaldiff * 100)  / (a.width() * a.height());
  if (totaldiff >= 0.01)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images have "
        << totaldiff << "% differencies \n" << Qt::endl;
    #else
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images have "
        << totaldiff << "% differencies \n" << endl;
    #endif
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  return true;
}
