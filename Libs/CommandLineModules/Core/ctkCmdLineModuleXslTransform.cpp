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
#include <QXmlQuery>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlFormatter>

// CTK includes
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleXmlMsgHandler_p.h"

//----------------------------------------------------------------------------
class ctkCmdLineModuleXslTransformPrivate
{
public:

  ctkCmdLineModuleXslTransformPrivate(QIODevice *output)
    : Validate(false)
    , Format(false)
    , OutputSchema(0)
    , Transformation(0)
    , Output(output)
    , XslTransform(QXmlQuery::XSLT20)
  {
    this->XslTransform.setMessageHandler(&this->MsgHandler);
  }

  bool validateOutput();

  bool Validate;
  bool Format;

  QIODevice* OutputSchema;
  QIODevice* Transformation;
  QIODevice* Output;

  QXmlQuery XslTransform;
  QList<QIODevice*> ExtraTransformations;
  ctkCmdLineModuleXmlMsgHandler MsgHandler;

  QString ErrorStr;
};

//----------------------------------------------------------------------------
bool ctkCmdLineModuleXslTransformPrivate::validateOutput()
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

//----------------------------------------------------------------------------
ctkCmdLineModuleXslTransform::ctkCmdLineModuleXslTransform(QIODevice *input, QIODevice *output)
  : ctkCmdLineModuleXmlValidator(input)
  , d(new ctkCmdLineModuleXslTransformPrivate(output))
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleXslTransform::~ctkCmdLineModuleXslTransform()
{
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setOutput(QIODevice* output)
{
  d->Output = output;
}

//----------------------------------------------------------------------------
QIODevice* ctkCmdLineModuleXslTransform::output() const
{
  return d->Output;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setOutputSchema(QIODevice *output)
{
  d->OutputSchema = output;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleXslTransform::formatXmlOutput() const
{
  return d->Format;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setFormatXmlOutput(bool format)
{
  d->Format = format;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleXslTransform::transform()
{
  d->ErrorStr.clear();

  if (!d->Output)
  {
    d->ErrorStr = "No output device set";
    return false;
  }
  QIODevice* inputDevice = this->input();
  if (!inputDevice)
  {
    d->ErrorStr = "No input set for deriving an output.";
    return false;
  }
  else if (!(inputDevice->openMode() & QIODevice::ReadOnly))
  {
    inputDevice->open(QIODevice::ReadOnly);
  }
  inputDevice->reset();


  if (!d->XslTransform.setFocus(inputDevice))
  {
    QString msg("Error transforming XML input: %1");
    d->ErrorStr = msg.arg(d->MsgHandler.statusMessage());
    return false;
  }

  if (!d->Transformation)
  {
    d->ErrorStr = "No XSL transformation set.";
    return false;
  }

  d->Transformation->open(QIODevice::ReadOnly);
  QString query(d->Transformation->readAll());
  QString extra;
  foreach(QIODevice* extraIODevice, d->ExtraTransformations)
  {
    extraIODevice->open(QIODevice::ReadOnly);
    extra += extraIODevice->readAll();
  }
  query.replace("<!-- EXTRA TRANSFORMATIONS -->", extra);
#if 0
  qDebug() << query;
#endif
  d->XslTransform.setQuery(query);

  bool closeOutput = false;
  if (!(d->Output->openMode() & QIODevice::WriteOnly))
  {
    d->Output->open(QIODevice::WriteOnly);
    closeOutput = true;
  }

  QScopedPointer<QXmlSerializer> xmlSerializer;
  if (d->Format)
  {
    xmlSerializer.reset(new QXmlFormatter(d->XslTransform, d->Output));
  }
  else
  {
    xmlSerializer.reset(new QXmlSerializer(d->XslTransform, d->Output));
  }

  if (!d->XslTransform.evaluateTo(xmlSerializer.data()))
  {
    QString msg("Error transforming XML input, at line %1, column %2: %3");
    d->ErrorStr = msg.arg(d->MsgHandler.line()).arg(d->MsgHandler.column())
        .arg(d->MsgHandler.statusMessage());
    return false;
  }

#if 0
  qDebug() << d->Output;
#endif

  if (closeOutput)
  {
    d->Output->close();
  }
  else
  {
    d->Output->reset();
  }

  if (d->Validate)
  {
    return d->validateOutput();
  }
  return true;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setXslTransformation(QIODevice *transformation)
{
  d->Transformation = transformation;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::bindVariable(const QString& name, const QVariant& value)
{
  d->XslTransform.bindVariable(name, value);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setXslExtraTransformation(QIODevice* transformation)
{
  QList<QIODevice*> transformations;
  transformations<<transformation;
  this->setXslExtraTransformations(transformations);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setXslExtraTransformations(const QList<QIODevice *>& transformations)
{
  d->ExtraTransformations = transformations;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleXslTransform::setValidateOutput(bool validate)
{
  d->Validate = validate;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleXslTransform::validateOutput() const
{
  return d->Validate;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleXslTransform::error() const
{
  return ctkCmdLineModuleXmlValidator::error() || !d->ErrorStr.isEmpty();
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleXslTransform::errorString() const
{
  QString errStr = this->ctkCmdLineModuleXmlValidator::errorString();
  if (errStr.isEmpty())
  {
    return d->ErrorStr;
  }
  return errStr;
}
