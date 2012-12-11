/*=============================================================================
  
  Library: CTK
  
  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics
    
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
    http://www.apache.org/licenses/LICENSE-2.0
    
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  
=============================================================================*/


#include <ctkCmdLineModuleXmlProgressWatcher.h>

#include "ctkCmdLineModuleSignalTester.h"

#include "ctkTest.h"

#include <QCoreApplication>
#include <QBuffer>
#include <QDataStream>
#include <QDebug>


namespace {

//-----------------------------------------------------------------------------
// Custom signal tester
class SignalTester : public ctkCmdLineModuleSignalTester
{
public:

  SignalTester() : accumulatedProgress(0)
  {}

  void filterStarted(const QString& name, const QString& comment)
  {
    ctkCmdLineModuleSignalTester::filterStarted(name, comment);
    if (name != "My Filter")
    {
      error = "Filter name does not match \"My Filter\" (got \"" + name + "\")";
      return;
    }
    if (comment != "Awesome filter")
    {
      error = "Filter comment does not match \"Awesome filter\" (got \"" + comment + "\")";
      return;
    }
  }

  void filterProgress(float progress, const QString& comment)
  {
    ctkCmdLineModuleSignalTester::filterProgress(progress, comment);
    accumulatedProgress += progress;
  }

  void filterFinished(const QString& name, const QString& comment)
  {
    ctkCmdLineModuleSignalTester::filterFinished(name, comment);
    if (name != "My Filter")
    {
      error = "Filter name does not match \"My Filter\" (got \"" + name + "\")";
      return;
    }
  }

  void filterXmlError(const QString& error)
  {
    ctkCmdLineModuleSignalTester::filterXmlError(error);
    this->error = error;
  }

  QString error;
  float accumulatedProgress;
};

}

//-----------------------------------------------------------------------------
class ctkCmdLineModuleXmlProgressWatcherTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testSignalsAndValues();
  void testMalformedXml();
};

//-----------------------------------------------------------------------------
void ctkCmdLineModuleXmlProgressWatcherTester::testSignalsAndValues()
{
  // Test data
  QByteArray filterStart = "<filter-start>\n"
                             "<filter-name>My Filter</filter-name>\n"
                             "<filter-comment>Awesome filter</filter-comment>\n"
                           "</filter-start>\n";
  QString filterProgress = "<filter-progress>%1</filter-progress>\n";
  QByteArray filterEnd = "<filter-end>\n"
                           "<filter-name>My Filter</filter-name>\n"
                           "<filter-time>23</filter-time>\n"
                         "</filter-end>";

  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  ctkCmdLineModuleXmlProgressWatcher progressWatcher(&buffer);

  SignalTester signalTester;
  signalTester.connect(&progressWatcher, SIGNAL(filterStarted(QString,QString)), &signalTester, SLOT(filterStarted(QString,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterProgress(float,QString)), &signalTester, SLOT(filterProgress(float,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterFinished(QString,QString)), &signalTester, SLOT(filterFinished(QString,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterXmlError(QString)), &signalTester, SLOT(filterXmlError(QString)));

  buffer.write(filterStart);
  buffer.write(filterProgress.arg(0.3).toLatin1());
  buffer.write(filterProgress.arg(0.6).toLatin1());
  buffer.write(filterProgress.arg(0.9).toLatin1());
  buffer.write(filterEnd);

  QCoreApplication::processEvents();

  QList<QString> expectedSignals;
  expectedSignals << "filter.started";
  expectedSignals << "filter.progress";
  expectedSignals << "filter.progress";
  expectedSignals << "filter.progress";
  expectedSignals << "filter.finished";

  if (!signalTester.error.isEmpty())
  {
    qDebug() << signalTester.error;
    QFAIL("XML parsing error");
  }

  QVERIFY(signalTester.checkSignals(expectedSignals));

  QCOMPARE(signalTester.accumulatedProgress, 1.8f);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleXmlProgressWatcherTester::testMalformedXml()
{
  // Test data
  QByteArray filterOutput = "<filter-start>\n"
                              "<filter-name>My Filter</filter-name>\n"
                              "<filter-comment>Awesome filter</filter-comment>\n"
                              "chunk<some-tag>...</some-tag>\n"
                              "<filter-progress>0.2</filter-progress>\n"
                            "</filter-start>\n"
                            "<filter-progress>0.5</filter-progress>\n"
                            "<filter-end>\n"

                              "<filter-name>My Filter</filter-name>\n"
                              "<filter-time>23</filter-time>\n"
                            "</filter-end>";

  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  ctkCmdLineModuleXmlProgressWatcher progressWatcher(&buffer);

  SignalTester signalTester;
  signalTester.connect(&progressWatcher, SIGNAL(filterStarted(QString,QString)), &signalTester, SLOT(filterStarted(QString,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterProgress(float,QString)), &signalTester, SLOT(filterProgress(float,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterFinished(QString,QString)), &signalTester, SLOT(filterFinished(QString,QString)));
  signalTester.connect(&progressWatcher, SIGNAL(filterXmlError(QString)), &signalTester, SLOT(filterXmlError(QString)));

  buffer.write(filterOutput);

  QCoreApplication::processEvents();

  QList<QString> expectedSignals;
  expectedSignals << "filter.xmlError";
  expectedSignals << "filter.started";
  expectedSignals << "filter.progress";
  expectedSignals << "filter.finished";

  QVERIFY(!signalTester.error.isEmpty());
  qDebug() << signalTester.error;

  QVERIFY(signalTester.checkSignals(expectedSignals));

  QCOMPARE(signalTester.accumulatedProgress, 0.5f);
}


// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleXmlProgressWatcherTest)
#include "moc_ctkCmdLineModuleXmlProgressWatcherTest.cpp"
