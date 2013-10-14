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
#include <QBuffer>
#include <QCoreApplication>
#include <QList>
#include <QString>

// CTK includes
#include "ctkCmdLineModuleFrontendFactoryQtGui.h"
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkCmdLineModuleQtXslTransformTester: public QObject
{
  Q_OBJECT
private slots:

  void initTestCase();

  void testTransform();
  void testTransform_data();

  void testBindVariable();
  void testBindVariable_data();

  void testXslExtraTransformation();
  void testXslExtraTransformation_data();
};

QString invalidXml =
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
  "<executable>";

QString header =
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
  "<executable>"
  " <category/>"
  " <title>A Test</title>"
  " <description><![CDATA[No parameter.]]></description>"
  " <version>0.1.0</version>"
  " <documentation-url/>"
  " <license/>"
  " <contributor></contributor>"
  " <acknowledgements/>";

QString footer =
  "</executable>\n";

QString mainWidgetHeader =
  "<ui version=\"4.0\">\n"
  "    <class>ATest</class>\n"
  "    <widget class=\"QWidget\" name=\"executable:A Test\">\n"
  "        <layout class=\"QVBoxLayout\">\n";

QString mainWidgetFooter =
  "            <item>\n"
  "                <spacer name=\"verticalSpacer\">\n"
  "                    <property name=\"orientation\">\n"
  "                        <enum>Qt::Vertical</enum>\n"
  "                    </property>\n"
  "                </spacer>\n"
  "            </item>\n"
  "        </layout>\n"
  "    </widget>\n"
  "    <connections/>\n"
  "</ui>\n";

QString parametersHeader =
  " <parameters>"
  "  <label>Parameters</label>"
  "  <description><![CDATA[Parameters]]></description>";

QString parametersFooter =
  " </parameters>\n";

QString parametersWidgetHeader =
  "            <item>\n"
  "                <widget class=\"ctkCollapsibleGroupBox\" name=\"paramGroup:Parameters\">\n"
  "                    <property name=\"title\">\n"
  "                        <string>Parameters</string>\n"
  "                    </property>\n"
  "                    <property name=\"toolTip\">\n"
  "                        <string>Parameters</string>\n"
  "                    </property>\n"
  "                    <property name=\"checked\">\n"
  "                        <bool>true</bool>\n"
  "                    </property>\n"
  ;

QString parametersWidgetEmptyLayout =
  "                    <layout class=\"QVBoxLayout\" name=\"paramContainerLayout:Parameters\">\n"
  "                        <item>\n"
  "                            <widget class=\"QWidget\" name=\"paramContainer:Parameters\">\n"
  "                                <layout class=\"QGridLayout\"/>\n"
  "                            </widget>\n"
  "                        </item>\n"
  "                    </layout>\n";

QString parametersLayoutHeader =
  "                    <layout class=\"QVBoxLayout\" name=\"paramContainerLayout:Parameters\">\n"
  "                        <item>\n"
  "                            <widget class=\"QWidget\" name=\"paramContainer:Parameters\">\n"
  "                                <layout class=\"QGridLayout\">\n";

QString parametersLayoutFooter =
  "                                </layout>\n"
  "                            </widget>\n"
  "                        </item>\n"
  "                    </layout>\n";

QString parametersWidgetFooter =
  "                </widget>\n"
  "            </item>\n"
  ;
QString integer =
  "<integer>"
  " <name>integer</name>"
  " <flag>-i</flag>"
  " <longflag>--integer</longflag>"
  " <description><![CDATA[Integer description]]></description>"
  " <label>Integer</label>"
  " <default>1</default>"
  "</integer>"
  ;

QString integerWidgetLabel =
  "                                    <item row=\"0\" column=\"0\">\n"
  "                                        <widget class=\"QLabel\">\n"
  "                                            <property name=\"sizePolicy\">\n"
  "                                                <sizepolicy hsizetype=\"Fixed\" vsizetype=\"Preferred\">\n"
  "                                                    <horstretch>0</horstretch>\n"
  "                                                    <verstretch>0</verstretch>\n"
  "                                                </sizepolicy>\n"
  "                                            </property>\n"
  "                                            <property name=\"text\">\n"
  "                                                <string>Integer</string>\n"
  "                                            </property>\n"
  "                                        </widget>\n"
  "                                    </item>\n"
  ;
QString integerWidgetSpinBoxHeader =
  "                                    <item row=\"0\" column=\"1\">\n"
  ;
QString integerWidgetSpinBox =
  "                                        <widget class=\"QSpinBox\" name=\"parameter:integer\">\n"
  "                                            <property name=\"minimum\">\n"
  "                                                <number>-999999999</number>\n"
  "                                            </property>\n"
  "                                            <property name=\"maximum\">\n"
  "                                                <number>999999999</number>\n"
  "                                            </property>\n"
  "                                            <property name=\"toolTip\">\n"
  "                                                <string>Integer description</string>\n"
  "                                            </property>\n"
  "                                            <property name=\"parameter:valueProperty\">\n"
  "                                                <string>value</string>\n"
  "                                            </property>\n"
  "                                            <property name=\"value\">\n"
  "                                                <number>1</number>\n"
  "                                            </property>\n"
  "                                        </widget>\n"
  ;
QString integerWidgetSpinBoxFooter =
  "                                    </item>\n"
  ;


// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::initTestCase()
{
  // Introduce a dummy linker dependency to CTKCommandLineModulesFrontendQtGui to
  // get access to the ctkCmdLineModuleXmlToQtUi.xsl resource.
  ctkCmdLineModuleFrontendFactoryQtGui guiFactory;
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testTransform()
{
  ctkCmdLineModuleXslTransform transformer;

  QFile transformation(":/ctkCmdLineModuleXmlToQtUi.xsl");
  transformer.setXslTransformation(&transformation);

  QFETCH(QString, input);
  QByteArray inputByteArray = input.toUtf8();
  QBuffer inputBuffer(&inputByteArray);
  transformer.setInput(&inputBuffer);

  QBuffer output;
  output.open(QBuffer::ReadWrite);
  transformer.setOutput(&output);

  transformer.setFormatXmlOutput(true);

  QFETCH(bool, expectedSuccess);
  bool success = transformer.transform();
  if (!success)
    {
    qDebug() << transformer.errorString();
    QCOMPARE(transformer.error(), true);
    QVERIFY(!transformer.errorString().isEmpty());
    }
  QCOMPARE(success, expectedSuccess);

  QFETCH(QString, expectedOutput);
  QCOMPARE(QString(output.readAll()), expectedOutput);
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testTransform_data()
{
  QTest::addColumn<QString>("input");
  QTest::addColumn<bool>("expectedSuccess");
  QTest::addColumn<QString>("expectedOutput");

  QTest::newRow("null") << QString() << false << QString();
  QTest::newRow("empty") << QString("") << false << QString();
  QTest::newRow("invalidXml") << invalidXml << false << QString();
  QString noParameter = header + footer;
  QString noParameterUi = mainWidgetHeader + mainWidgetFooter;
  QTest::newRow("no parameter") << noParameter << true << noParameterUi;

  QString justParameters =
    header
    + parametersHeader
    + parametersFooter
    + footer;
  QString justParametersUi =
    mainWidgetHeader
    + parametersWidgetHeader
    + parametersWidgetEmptyLayout
    + parametersWidgetFooter
    + mainWidgetFooter;
  QTest::newRow("just parameters") << justParameters << true << justParametersUi;

  QString integerParameter =
    header
    + parametersHeader
    + integer
    + parametersFooter
    + footer;
  QString integerParameterUi =
    mainWidgetHeader
    + parametersWidgetHeader
    + parametersLayoutHeader
    + integerWidgetLabel
    + integerWidgetSpinBoxHeader
    + integerWidgetSpinBox
    + integerWidgetSpinBoxFooter
    + parametersLayoutFooter
    + parametersWidgetFooter
    + mainWidgetFooter;
  QTest::newRow("integer") << integerParameter << true << integerParameterUi;

}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testBindVariable()
{
  ctkCmdLineModuleXslTransform transformer;

  QFile transformation(":/ctkCmdLineModuleXmlToQtUi.xsl");
  transformer.setXslTransformation(&transformation);

  QFETCH(QString, input);
  QByteArray inputArray(input.toUtf8());
  QBuffer inputBuffer(&inputArray);
  transformer.setInput(&inputBuffer);

  QBuffer output;
  output.open(QBuffer::ReadWrite);
  transformer.setOutput(&output);

  transformer.setFormatXmlOutput(true);

  QFETCH(QString, variableName);
  QFETCH(QString, variableValue);
  transformer.bindVariable(variableName, variableValue);

  transformer.transform();

  QFETCH(QString, expectedOutput);
  QCOMPARE(QString(transformer.output()->readAll()), expectedOutput);
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testBindVariable_data()
{
  QTest::addColumn<QString>("input");
  QTest::addColumn<QString>("variableName");
  QTest::addColumn<QString>("variableValue");
  QTest::addColumn<QString>("expectedOutput");

  QString integerParameter =
    header
    + parametersHeader
    + integer
    + parametersFooter
    + footer;
  QString integerParameterUi =
    mainWidgetHeader
    + parametersWidgetHeader
    + parametersLayoutHeader
    + integerWidgetLabel
    + integerWidgetSpinBoxHeader
    + integerWidgetSpinBox
    + integerWidgetSpinBoxFooter
    + parametersLayoutFooter
    + parametersWidgetFooter
    + mainWidgetFooter;
  integerParameterUi.replace("QSpinBox", "ctkSliderWidget");
  QTest::newRow("QSpinBox -> ctkDoubleSpinBox") << integerParameter
                                          << QString("integerWidget")
                                          << QString("ctkSliderWidget")
                                          << integerParameterUi;
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testXslExtraTransformation()
{
  ctkCmdLineModuleXslTransform transformer;

  QFile transformation(":/ctkCmdLineModuleXmlToQtUi.xsl");
  transformer.setXslTransformation(&transformation);

  QFETCH(QString, input);
  QByteArray inputArray(input.toUtf8());
  QBuffer inputBuffer(&inputArray);
  transformer.setInput(&inputBuffer);

  QBuffer output;
  output.open(QBuffer::ReadWrite);
  transformer.setOutput(&output);

  transformer.setFormatXmlOutput(true);

  QFETCH(QString, extra);
  QByteArray extraTransformationArray(extra.toUtf8());
  QBuffer extraTransformationBuffer(&extraTransformationArray);
  transformer.setXslExtraTransformation(&extraTransformationBuffer);

  transformer.transform();

  QFETCH(QString, expectedOutput);
  //qDebug() << transformer.output();
  //qDebug() << expectedOutput;
  QCOMPARE(QString(transformer.output()->readAll()), expectedOutput);
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleQtXslTransformTester::testXslExtraTransformation_data()
{
  QString extra =
    "<xsl:template match=\"parameters/integer\" priority=\"1\">\n"
    "<xsl:call-template name=\"gridItemWithLabel\"/>\n"
    "<item  row=\"{position()-1}\" column=\"1\">\n"
    "  <layout class=\"QHBoxLayout\">\n"
    "    <item>\n"
    "      <widget class=\"QSlider\"  name=\"parameter:{name}Slider\">\n"
    "        <xsl:if test=\"not(constraints)\">\n"
    "          <property name=\"minimum\">\n"
    "            <number>-999999999</number>\n"
    "          </property>\n"
    "          <property name=\"maximum\">\n"
    "            <number>999999999</number>\n"
    "          </property>\n"
    "        </xsl:if>\n"
    "        <xsl:call-template name=\"commonWidgetProperties\"/>\n"
    "      </widget>\n"
    "    </item>"
    "    <item>\n"
    "      <widget class=\"QSpinBox\"  name=\"parameter:{name}\">\n"
    "        <xsl:if test=\"not(constraints)\">\n"
    "          <property name=\"minimum\">\n"
    "            <number>-999999999</number>\n"
    "          </property>\n"
    "          <property name=\"maximum\">\n"
    "            <number>999999999</number>\n"
    "          </property>\n"
    "        </xsl:if>\n"
    "        <xsl:call-template name=\"commonWidgetProperties\"/>\n"
    "      </widget>\n"
    "    </item>\n"
    "  </layout>\n"
    "</item>\n"
    "</xsl:template>\n"
    ;
  QString integerWidgetSliderSpinBox =
    "                                        <layout class=\"QHBoxLayout\">\n"
    "                                            <item>\n"
    "                                                <widget class=\"QSlider\" name=\"parameter:integerSlider\">\n"
    "                                                    <property name=\"minimum\">\n"
    "                                                        <number>-999999999</number>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"maximum\">\n"
    "                                                        <number>999999999</number>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"toolTip\">\n"
    "                                                        <string>Integer description</string>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"parameter:valueProperty\">\n"
    "                                                        <string>value</string>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"value\">\n"
    "                                                        <number>1</number>\n"
    "                                                    </property>\n"
    "                                                </widget>\n"
    "                                            </item>\n"
    "                                            <item>\n"
    "                                                <widget class=\"QSpinBox\" name=\"parameter:integer\">\n"
    "                                                    <property name=\"minimum\">\n"
    "                                                        <number>-999999999</number>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"maximum\">\n"
    "                                                        <number>999999999</number>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"toolTip\">\n"
    "                                                        <string>Integer description</string>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"parameter:valueProperty\">\n"
    "                                                        <string>value</string>\n"
    "                                                    </property>\n"
    "                                                    <property name=\"value\">\n"
    "                                                        <number>1</number>\n"
    "                                                    </property>\n"
    "                                                </widget>\n"
    "                                            </item>\n"
    "                                        </layout>\n"
    ;
  QTest::addColumn<QString>("input");
  QTest::addColumn<QString>("extra");
  QTest::addColumn<QString>("expectedOutput");

  QString integerParameter =
    header
    + parametersHeader
    + integer
    + parametersFooter
    + footer;
  QString integerParameterUi =
    mainWidgetHeader
    + parametersWidgetHeader
    + parametersLayoutHeader
    + integerWidgetLabel
    + integerWidgetSpinBoxHeader
    + integerWidgetSliderSpinBox
    + integerWidgetSpinBoxFooter
    + parametersLayoutFooter
    + parametersWidgetFooter
    + mainWidgetFooter;
  QTest::newRow("QSpinBox -> QSlider+QSpinBox") << integerParameter
                                          << extra
                                          << integerParameterUi;

}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleQtXslTransformTest)
#include "moc_ctkCmdLineModuleQtXslTransformTest.cpp"
