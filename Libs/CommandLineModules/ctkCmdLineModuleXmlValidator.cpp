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

#include <QFile>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlQuery>
#include <QAbstractMessageHandler>

#include <QDebug>

class _MessageHandler : public QAbstractMessageHandler
{
public:

  QString statusMessage() const { return m_description; }
  int line() const { return m_sourceLocation.line(); }
  int column() const { return m_sourceLocation.column(); }

protected:
  virtual void handleMessage(QtMsgType type, const QString& description,
                             const QUrl& identifier, const QSourceLocation& sourceLocation)
  {
    Q_UNUSED(identifier)

    m_messageType = type;
    m_sourceLocation = sourceLocation;

    QXmlStreamReader reader(description);
    m_description.clear();
    m_description.reserve(description.size());
    while(!reader.atEnd())
    {
      reader.readNext();

      switch(reader.tokenType())
      {
      case QXmlStreamReader::Characters:
      {
        m_description.append(reader.text().toString());
        continue;
      }
      case QXmlStreamReader::StartElement:
        /* Fallthrough, */
      case QXmlStreamReader::EndElement:
        /* Fallthrough, */
      case QXmlStreamReader::StartDocument:
        /* Fallthrough, */
      case QXmlStreamReader::EndDocument:
        continue;
      default:
        Q_ASSERT_X(false, Q_FUNC_INFO,
                   "Unexpected node.");
      }
    }
  }

private:
  QtMsgType m_messageType;
  QString m_description;
  QSourceLocation m_sourceLocation;
};

ctkCmdLineModuleXmlValidator::ctkCmdLineModuleXmlValidator(QIODevice *input)
  : _input(input), _inputSchema(0), _outputSchema(0), _transformation(0)
{
}

void ctkCmdLineModuleXmlValidator::setInput(QIODevice *input)
{
  _input = input;
}

QString ctkCmdLineModuleXmlValidator::output()
{
  return _output;
}

void ctkCmdLineModuleXmlValidator::setInputSchema(QIODevice *input)
{
  _inputSchema = input;
}

void ctkCmdLineModuleXmlValidator::setOutputSchema(QIODevice *output)
{
  _outputSchema = output;
}

void ctkCmdLineModuleXmlValidator::setXSLTransformation(QIODevice *transformation)
{
  _transformation = transformation;
}

bool ctkCmdLineModuleXmlValidator::validate()
{
  return validateXMLInput() && validateXSLTOutput();
}

bool ctkCmdLineModuleXmlValidator::validateXMLInput()
{
  _errorStr.clear();

  if (!_input)
  {
    _errorStr = "No input set for validation.";
    return false;
  }

  QIODevice* inputSchema = _inputSchema;
  QScopedPointer<QIODevice> defaultInputSchema(new QFile(":/ctkCmdLineModule.xsd"));
  if (!inputSchema)
  {
    inputSchema = defaultInputSchema.data();
    inputSchema->open(QIODevice::ReadOnly);
  }

  _MessageHandler errorHandler;

  QXmlSchema schema;
  schema.setMessageHandler(&errorHandler);

  if (!schema.load(inputSchema))
  {
    QString msg("Invalid input schema at line %1, column %2: %3");
    _errorStr = msg.arg(errorHandler.line()).arg(errorHandler.column()).arg(errorHandler.statusMessage());
    return false;
  }

  QXmlSchemaValidator validator(schema);

  if (!validator.validate(_input))
  {
    QString msg("Error validating CLI XML description, at line %1, column %2: %3");
    _errorStr = msg.arg(errorHandler.line()).arg(errorHandler.column())
                .arg(errorHandler.statusMessage());
    return false;
  }

  return true;
}

bool ctkCmdLineModuleXmlValidator::validateXSLTOutput()
{
  _errorStr.clear();
  _output.clear();

  if (!_input)
  {
    _errorStr = "No input set for deriving an output.";
    return false;
  }
  else if (!(_input->openMode() & QIODevice::ReadOnly))
  {
    _input->open(QIODevice::ReadOnly);
  }
  _input->reset();

  _MessageHandler msgHandler;

  QXmlQuery xslTransform(QXmlQuery::XSLT20);
  xslTransform.setMessageHandler(&msgHandler);
  if (!xslTransform.setFocus(_input))
  {
    QString msg("Error transforming CLI XML description: %1");
    _errorStr = msg.arg(msgHandler.statusMessage());
    return false;
  }

  QIODevice* transformation = _transformation;
  QScopedPointer<QIODevice> defaultTransform(new QFile(":/ctkCmdLineModuleXmlToQtUi.xsl"));
  if (!transformation)
  {
    transformation = defaultTransform.data();
    transformation->open(QIODevice::ReadOnly);
  }
  xslTransform.setQuery(transformation);

  if (!xslTransform.evaluateTo(&_output))
  {
    _output.clear();
    QString msg("Error transforming CLI XML description, at line %1, column %2: %3");
    _errorStr = msg.arg(msgHandler.line()).arg(msgHandler.column())
                .arg(msgHandler.statusMessage());
    return false;
  }

  QIODevice* outputSchema = _outputSchema;

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
  schema.setMessageHandler(&msgHandler);

  if (!schema.load(outputSchema))
  {
    QString msg("Invalid output schema at line %1, column %2: %3");
    _errorStr = msg.arg(msgHandler.line()).arg(msgHandler.column()).arg(msgHandler.statusMessage());
    return false;
  }

  QXmlSchemaValidator validator(schema);
  validator.setMessageHandler(&msgHandler);

  QByteArray outputData;
  outputData.append(_output);

  if (!validator.validate(outputData))
  {
    QString msg("Error validating transformed CLI XML description, at line %1, column %2: %3");
    _errorStr = msg.arg(msgHandler.line()).arg(msgHandler.column())
                .arg(msgHandler.statusMessage());
    return false;
  }

  return true;
}

bool ctkCmdLineModuleXmlValidator::error() const
{
  return !_errorStr.isEmpty();
}

QString ctkCmdLineModuleXmlValidator::errorString() const
{
  return _errorStr;
}
