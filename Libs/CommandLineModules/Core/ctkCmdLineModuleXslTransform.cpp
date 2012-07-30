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

// Qt includes
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlFormatter>

// CTK includes
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleXmlMsgHandler_p.h"

ctkCmdLineModuleXslTransform::ctkCmdLineModuleXslTransform(QIODevice *input, QIODevice *output)
  : ctkCmdLineModuleXmlValidator(input)
  , Validate(false)
  , Format(false)
  , OutputSchema(0)
  , Transformation(0)
  , Output(output)
  , XslTransform(QXmlQuery::XSLT20)
  , MsgHandler(0)
{
  this->MsgHandler = new ctkCmdLineModuleXmlMsgHandler();
  this->XslTransform.setMessageHandler(this->MsgHandler);

  this->bindVariable("executableWidget", QVariant(QString("QWidget")));
  this->bindVariable("parametersWidget", QVariant(QString("ctkCollapsibleGroupBox")));
  this->bindVariable("booleanWidget", QVariant(QString("QCheckBox")));
  this->bindVariable("integerWidget", QVariant(QString("QSpinBox")));
  this->bindVariable("floatingWidget", QVariant(QString("QDoubleSpinBox")));
  this->bindVariable("vectorWidget", QVariant(QString("QLineEdit")));
  this->bindVariable("enumWidget", QVariant(QString("QComboBox")));
  this->bindVariable("imageWidget", QVariant(QString("ctkPathLineEdit")));
  this->bindVariable("directoryWidget", QVariant(QString("ctkPathLineEdit")));
  this->bindVariable("pointWidget", QVariant(QString("ctkCoordinatesWidget")));
  this->bindVariable("unsupportedWidget", QVariant(QString("QLabel")));
}

ctkCmdLineModuleXslTransform::~ctkCmdLineModuleXslTransform()
{
  delete this->MsgHandler;
}

void ctkCmdLineModuleXslTransform::setOutput(QIODevice* output)
{
  this->Output = output;
}

QIODevice* ctkCmdLineModuleXslTransform::output() const
{
  return this->Output;
}

void ctkCmdLineModuleXslTransform::setOutputSchema(QIODevice *output)
{
  this->OutputSchema = output;
}

bool ctkCmdLineModuleXslTransform::formatXmlOutput() const
{
  return this->Format;
}

void ctkCmdLineModuleXslTransform::setFormatXmlOutput(bool format)
{
  this->Format = format;
}

bool ctkCmdLineModuleXslTransform::transform()
{
  this->ErrorStr.clear();

  if (!Output)
  {
    this->ErrorStr = "No output device set";
    return false;
  }
  QIODevice* inputDevice = this->input();
  if (!inputDevice)
  {
    this->ErrorStr = "No input set for deriving an output.";
    return false;
  }
  else if (!(inputDevice->openMode() & QIODevice::ReadOnly))
  {
    inputDevice->open(QIODevice::ReadOnly);
  }
  inputDevice->reset();


  if (!this->XslTransform.setFocus(inputDevice))
  {
    QString msg("Error transforming XML input: %1");
    this->ErrorStr = msg.arg(this->MsgHandler->statusMessage());
    return false;
  }

  QIODevice* transformation = this->Transformation;
  QScopedPointer<QIODevice> defaultTransform(new QFile(":/ctkCmdLineModuleXmlToQtUi.xsl"));
  if (!transformation)
  {
    transformation = defaultTransform.data();
    transformation->open(QIODevice::ReadOnly);
  }
  QString query(transformation->readAll());
  QString extra;
  foreach(QIODevice* extraIODevice, this->ExtraTransformations)
    {
    extraIODevice->open(QIODevice::ReadOnly);
    extra += extraIODevice->readAll();
    }
  query.replace("<!-- EXTRA TRANSFORMATIONS -->", extra);
#if 0
  qDebug() << query;
#endif
  this->XslTransform.setQuery(query);

  bool closeOutput = false;
  if (!(this->Output->openMode() & QIODevice::WriteOnly))
  {
    this->Output->open(QIODevice::WriteOnly);
    closeOutput = true;
  }

  QScopedPointer<QXmlSerializer> xmlSerializer;
  if (Format)
  {
    xmlSerializer.reset(new QXmlFormatter(this->XslTransform, this->Output));
  }
  else
  {
    xmlSerializer.reset(new QXmlSerializer(this->XslTransform, this->Output));
  }

  if (!this->XslTransform.evaluateTo(xmlSerializer.data()))
  {
    QString msg("Error transforming XML input, at line %1, column %2: %3");
    this->ErrorStr = msg.arg(this->MsgHandler->line()).arg(this->MsgHandler->column())
      .arg(this->MsgHandler->statusMessage());
    return false;
  }

#if 0
  qDebug() << this->Output;
#endif

  if (closeOutput)
  {
    this->Output->close();
  }
  else
  {
    this->Output->reset();
  }

  if (this->Validate)
  {
    return this->validateOutput();
  }
  return true;
}

void ctkCmdLineModuleXslTransform::setXslTransformation(QIODevice *transformation)
{
  this->Transformation = transformation;
}

void ctkCmdLineModuleXslTransform::bindVariable(const QString& name, const QVariant& value)
{
  this->XslTransform.bindVariable(name, value);
}

void ctkCmdLineModuleXslTransform::setXslExtraTransformation(QIODevice* transformation)
{
  QList<QIODevice*> transformations;
  transformations<<transformation;
  this->setXslExtraTransformations(transformations);
}

void ctkCmdLineModuleXslTransform::setXslExtraTransformations(const QList<QIODevice *>& transformations)
{
  this->ExtraTransformations = transformations;
}

void ctkCmdLineModuleXslTransform::setValidateOutput(bool validate)
{
  this->Validate = validate;
}

bool ctkCmdLineModuleXslTransform::validateOutput()
{
  this->ErrorStr.clear();

  QIODevice* outputSchema = this->OutputSchema;

  // If there is no custom schema for validating the output, we just return.
  // The QtDesigner.xsd schema below (which should be the default) exhausts
  // the memory during validation.
  if (!outputSchema) return true;

  QScopedPointer<QIODevice> defaultOutputSchema(new QFile(":/QtDesigner.xsd"));
  if (!outputSchema)
  {
    outputSchema = defaultOutputSchema.data();
    outputSchema->open(QIODevice::ReadOnly);
  }
  outputSchema->reset();

  QXmlSchema schema;

  ctkCmdLineModuleXmlMsgHandler msgHandler;
  schema.setMessageHandler(&msgHandler);

  if (!schema.load(outputSchema))
  {
    QString msg("Invalid output schema at line %1, column %2: %3");
    ErrorStr = msg.arg(msgHandler.line()).arg(msgHandler.column()).arg(msgHandler.statusMessage());
    return false;
  }

  QXmlSchemaValidator validator(schema);
  validator.setMessageHandler(&msgHandler);

  if (!validator.validate(Output))
  {
    QString msg("Error validating transformed XML input, at line %1, column %2: %3");
    ErrorStr = msg.arg(msgHandler.line()).arg(msgHandler.column())
                .arg(msgHandler.statusMessage());
    return false;
  }

  return true;
}

bool ctkCmdLineModuleXslTransform::error() const
{
  return ctkCmdLineModuleXmlValidator::error() || !this->ErrorStr.isEmpty();
}

QString ctkCmdLineModuleXslTransform::errorString() const
{
  QString errStr = this->ctkCmdLineModuleXmlValidator::errorString();
  if (errStr.isEmpty())
  {
    return this->ErrorStr;
  }
  return errStr;
}
