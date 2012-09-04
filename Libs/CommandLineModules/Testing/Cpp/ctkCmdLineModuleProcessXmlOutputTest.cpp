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

#include "ctkCmdLineModuleXmlValidator.h"

#include "ctkTest.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>


//-----------------------------------------------------------------------------
class ctkCmdLineModuleProcessXmlOutputTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testValidXmlOutput();

};

//-----------------------------------------------------------------------------
void ctkCmdLineModuleProcessXmlOutputTester::testValidXmlOutput()
{
  QString processLocation = QCoreApplication::applicationDirPath() + "/ctkCmdLineModuleTestBed";
  QProcess process;
  process.start(processLocation, QStringList() << "--numOutputs" << "1" << "dummy");

  QVERIFY(process.waitForFinished());
  QByteArray output = process.readAllStandardOutput();
  QVERIFY(!output.isEmpty());
  output.prepend("<module-root>");
  output.append("</module-root>");

  QBuffer buffer(&output);
  buffer.open(QIODevice::ReadOnly);
  ctkCmdLineModuleXmlValidator xmlValidator(&buffer);

  QFile schema(":/ctkCmdLineModuleProcess.xsd");
  QVERIFY(schema.open(QIODevice::ReadOnly));
  xmlValidator.setInputSchema(&schema);

  QVERIFY(xmlValidator.validateInput());
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleProcessXmlOutputTest)
#include "moc_ctkCmdLineModuleProcessXmlOutputTest.cpp"
