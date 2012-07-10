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

#include "ctkCmdLineModuleXslTransform.h"

#include "ctkCmdLineModuleXmlMsgHandler_p.h"

#include <QFile>
#include <QBuffer>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlQuery>

#include <QDebug>


ctkCmdLineModuleXslTransform::ctkCmdLineModuleXslTransform(QIODevice *input)
  : ctkCmdLineModuleXmlValidator(input),
    OutputSchema(0), Transformation(0)
{
}

QString ctkCmdLineModuleXslTransform::output() const
{
  return Output;
}

void ctkCmdLineModuleXslTransform::setOutputSchema(QIODevice *output)
{
  OutputSchema = output;
}

bool ctkCmdLineModuleXslTransform::transform()
{
  QIODevice* inputDevice = input();
  if (!inputDevice)
  {
    ErrorStr = "No input set for deriving an output.";
    return false;
  }
  else if (!(inputDevice->openMode() & QIODevice::ReadOnly))
  {
    inputDevice->open(QIODevice::ReadOnly);
  }
  inputDevice->reset();

  ctkCmdLineModuleXmlMsgHandler msgHandler;

  QXmlQuery xslTransform(QXmlQuery::XSLT20);
  xslTransform.setMessageHandler(&msgHandler);
  if (!xslTransform.setFocus(inputDevice))
  {
    QString msg("Error transforming XML input: %1");
    ErrorStr = msg.arg(msgHandler.statusMessage());
    return false;
  }

  QIODevice* transformation = Transformation;
  QScopedPointer<QIODevice> defaultTransform(new QFile(":/ctkCmdLineModuleXmlToQtUi.xsl"));
  if (!transformation)
  {
    transformation = defaultTransform.data();
    transformation->open(QIODevice::ReadOnly);
  }
  xslTransform.setQuery(transformation);

  if (!xslTransform.evaluateTo(&Output))
  {
    Output.clear();
    QString msg("Error transforming XML input, at line %1, column %2: %3");
    ErrorStr = msg.arg(msgHandler.line()).arg(msgHandler.column())
        .arg(msgHandler.statusMessage());
    return false;
  }

  return true;
}

void ctkCmdLineModuleXslTransform::setXslTransformation(QIODevice *transformation)
{
  Transformation = transformation;
}

bool ctkCmdLineModuleXslTransform::validateOutput()
{
  ErrorStr.clear();

  if (Output.isEmpty())
  {
    // nothing to validate
    return true;
  }

  QIODevice* outputSchema = OutputSchema;

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

  QByteArray outputData;
  outputData.append(Output);

  if (!validator.validate(outputData))
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
  return ctkCmdLineModuleXmlValidator::error() || !ErrorStr.isEmpty();
}

QString ctkCmdLineModuleXslTransform::errorString() const
{
  QString errStr = ctkCmdLineModuleXmlValidator::errorString();
  if (errStr.isEmpty())
  {
    return ErrorStr;
  }
  return errStr;
}
