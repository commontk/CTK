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

#include "ctkCmdLineModuleXmlMsgHandler_p.h"

#include <QFile>
#include <QBuffer>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include <QDebug>

ctkCmdLineModuleXmlValidator::ctkCmdLineModuleXmlValidator(QIODevice *input)
  : Input(input), InputSchema(0)
{
}

void ctkCmdLineModuleXmlValidator::setInput(QIODevice *input)
{
  Input = input;
}

QIODevice* ctkCmdLineModuleXmlValidator::input() const
{
  return Input;
}

void ctkCmdLineModuleXmlValidator::setInputSchema(QIODevice *input)
{
  InputSchema = input;
}

bool ctkCmdLineModuleXmlValidator::validateInput()
{
  ErrorStr.clear();

  if (!Input)
  {
    ErrorStr = "No input set for validation.";
    return false;
  }

  QIODevice* inputSchema = InputSchema;
  QScopedPointer<QIODevice> defaultInputSchema(new QFile(":/ctkCmdLineModule.xsd"));
  if (!inputSchema)
  {
    inputSchema = defaultInputSchema.data();
    inputSchema->open(QIODevice::ReadOnly);
  }

  ctkCmdLineModuleXmlMsgHandler errorHandler;

  QXmlSchema schema;
  schema.setMessageHandler(&errorHandler);

  if (!schema.load(inputSchema))
  {
    QString msg("Invalid input schema at line %1, column %2: %3");
    ErrorStr = msg.arg(errorHandler.line()).arg(errorHandler.column()).arg(errorHandler.statusMessage());
    return false;
  }

  QXmlSchemaValidator validator(schema);

  if (!validator.validate(Input))
  {
    QString msg("Error validating CLI XML description, at line %1, column %2: %3");
    ErrorStr = msg.arg(errorHandler.line()).arg(errorHandler.column())
                .arg(errorHandler.statusMessage());
    return false;
  }

  return true;
}

bool ctkCmdLineModuleXmlValidator::error() const
{
  return !ErrorStr.isEmpty();
}

QString ctkCmdLineModuleXmlValidator::errorString() const
{
  return ErrorStr;
}
