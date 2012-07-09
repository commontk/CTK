/*=============================================================================

Library: CTK

Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

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
#include <QDebug>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QAbstractMessageHandler>
#include <QXmlStreamReader>

// CTK includes
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleParameterParsers_p.h"

// STD includes
#include <stdexcept>

class ctkCmdLineModuleXmlParser
{
public:

  ctkCmdLineModuleXmlParser(QIODevice* device, ctkCmdLineModuleDescription* md);
  ~ctkCmdLineModuleXmlParser();

  void validate();
  void doParse();

  void handleExecutableElement();
  void handleParametersElement();
  ctkCmdLineModuleParameter* handleParameterElement();

private:

  QIODevice* const _device;
  ctkCmdLineModuleDescription* _md;
  QXmlStreamReader _xmlReader;
  QHash<QString, ctkCmdLineModuleParameterParser*> _paramParsers;
};

namespace {

static const QString DESCRIPTION = "description";

}


// ----------------------------------------------------------------------------
ctkCmdLineModuleXmlParser::ctkCmdLineModuleXmlParser(QIODevice* device,
                                                       ctkCmdLineModuleDescription* md)
  : _device(device), _md(md)
{
  _paramParsers["integer"] = new ctkCmdLineModuleScalarParameterParser; // type="scalarType"/>
  _paramParsers["integer-vector"] = new ctkCmdLineModuleScalarVectorParameterParser; // type="scalarVectorType"/>
  _paramParsers["boolean"] = new ctkCmdLineModuleParameterParser; // type="paramType"/>
  _paramParsers["float"] = new ctkCmdLineModuleScalarParameterParser; // type="scalarType"/>
  _paramParsers["float-vector"] = new ctkCmdLineModuleScalarVectorParameterParser; // type="scalarVectorType"/>
  _paramParsers["double"] = new ctkCmdLineModuleScalarParameterParser; // type="scalarType"/>
  _paramParsers["double-vector"] = new ctkCmdLineModuleScalarVectorParameterParser; // type="scalarVectorType"/>
  _paramParsers["string"] = new ctkCmdLineModuleMultipleParameterParser; // type="multipleType"/>
  _paramParsers["string-vector"] = new ctkCmdLineModuleParameterParser; // type="paramType"/>
  _paramParsers["point"] = new ctkCmdLineModulePointParameterParser; // type="pointType"/>
  _paramParsers["region"] = new ctkCmdLineModulePointParameterParser; // type="pointType"/>
  _paramParsers["string-enumeration"] = new ctkCmdLineModuleEnumerationParameterParser; // type="enumerationType"/>
  _paramParsers["integer-enumeration"] = new ctkCmdLineModuleEnumerationParameterParser; // type="enumerationType"/>
  _paramParsers["float-enumeration"] = new ctkCmdLineModuleEnumerationParameterParser; // type="enumerationType"/>
  _paramParsers["double-enumeration"] = new ctkCmdLineModuleEnumerationParameterParser; // type="enumerationType"/>
  _paramParsers["file"] = new ctkCmdLineModuleFileParameterParser; // type="fileType"/>
  _paramParsers["directory"] = new ctkCmdLineModuleChannelParameterParser; // type="channelType"/>
  _paramParsers["image"] = new ctkCmdLineModuleImageParameterParser; // type="imageType"/>
  _paramParsers["geometry"] = new ctkCmdLineModuleGeometryParameterParser; // type="geometryType"/>
}

// ----------------------------------------------------------------------------
ctkCmdLineModuleXmlParser::~ctkCmdLineModuleXmlParser()
{
  qDeleteAll(_paramParsers.values());
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleXmlParser::validate()
{
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
  _MessageHandler errorHandler;

  QXmlSchema schema;
  schema.setMessageHandler(&errorHandler);
  schema.load(QUrl::fromLocalFile(":/ctkCmdLineModuleDescription.xsd"));

  bool res = schema.isValid();
  if (!res)
  {
    QString msg("Invalid Schema at line %1, column %2: %3");
    msg = msg.arg(errorHandler.line()).arg(errorHandler.column()).arg(errorHandler.statusMessage());
    throw std::runtime_error(msg.toStdString());
  }

  QXmlSchemaValidator validator(schema);
  _device->open(QIODevice::ReadOnly);
  res = validator.validate(_device);
  _device->close();

  if (!res)
  {
    QString msg("Error validating XML description, at line %1, column %2: %3");
    throw std::runtime_error(msg.arg(errorHandler.line())
                             .arg(errorHandler.column())
                             .arg(errorHandler.statusMessage()).toStdString());
  }

}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleXmlParser::doParse()
{
  _xmlReader.clear();
  _xmlReader.setDevice(_device);
  _device->open(QIODevice::ReadOnly);

  _xmlReader.readNextStartElement();
  this->handleExecutableElement();

  if (_xmlReader.hasError())
  {
    QString msg("Error parsing module description at line %1, column %2: %3");
    throw std::runtime_error(msg.arg(_xmlReader.lineNumber()).arg(_xmlReader.columnNumber())
                             .arg(_xmlReader.errorString()).toStdString());
  }
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleXmlParser::handleExecutableElement()
{
  while(_xmlReader.readNextStartElement())
  {
    QStringRef name = _xmlReader.name();

    if (name.compare("category", Qt::CaseInsensitive) == 0)
    {
      _md->setCategory(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("title", Qt::CaseInsensitive) == 0)
    {
      _md->setTitle(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("version", Qt::CaseInsensitive) == 0)
    {
      _md->setVersion(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("documentation-url", Qt::CaseInsensitive) == 0)
    {
      _md->setDocumentationURL(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("license", Qt::CaseInsensitive) == 0)
    {
      _md->setLicense(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("acknowledgements", Qt::CaseInsensitive) == 0)
    {
      _md->setAcknowledgements(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("contributor", Qt::CaseInsensitive) == 0)
    {
      _md->setContributor(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("description", Qt::CaseInsensitive) == 0)
    {
      _md->setDescription(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("parameters", Qt::CaseInsensitive) == 0)
    {
      this->handleParametersElement();
    }
    else
    {
      qCritical() << "Unknown element" << name << "encountered at line"
                  << _xmlReader.lineNumber() << ", column" << _xmlReader.columnNumber() << ".";
      _xmlReader.skipCurrentElement();
    }
  }
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleXmlParser::handleParametersElement()
{
  ctkCmdLineModuleParameterGroup* group = new ctkCmdLineModuleParameterGroup();

  group->setAdvanced(parseBooleanAttribute(_xmlReader.attributes().value("advanced")));

  while(_xmlReader.readNextStartElement())
  {
    QStringRef name = _xmlReader.name();

    if (name.compare("label", Qt::CaseInsensitive) == 0)
    {
      group->setLabel(_xmlReader.readElementText().trimmed());
    }
    else if (name.compare("description", Qt::CaseInsensitive) == 0)
    {
      group->setDescription(_xmlReader.readElementText().trimmed());
    }
    else
    {
      ctkCmdLineModuleParameter* parameter = this->handleParameterElement();
      if (parameter)
      {
        group->addParameter(parameter);
      }
    }
  }

  _md->addParameterGroup(group);
}

// ----------------------------------------------------------------------------
ctkCmdLineModuleParameter* ctkCmdLineModuleXmlParser::handleParameterElement()
{
  QString paramTag = _xmlReader.name().toString().toLower();
  ctkCmdLineModuleParameterParser* paramParser = _paramParsers[paramTag];
  if (paramParser == 0)
  {
    _xmlReader.skipCurrentElement();
    qCritical() << "No parser for element" << paramTag << "available, line"
                << _xmlReader.lineNumber() << ", column" << _xmlReader.columnNumber();
    return 0;
  }
  else
  {
    ctkCmdLineModuleParameter* moduleParam = paramParser->parse(_xmlReader);
    moduleParam->setTag(paramTag);
    return moduleParam;
  }
}

// ----------------------------------------------------------------------------
ctkCmdLineModuleDescription* ctkCmdLineModuleDescription::parse(QIODevice* device)
{
  ctkCmdLineModuleDescription* moduleDescription = new ctkCmdLineModuleDescription();
  ctkCmdLineModuleXmlParser parser(device, moduleDescription);

  try
  {
    // Verify the xml is correct
    parser.validate();

    parser.doParse();
  }
  catch(...)
  {
    delete moduleDescription;
    throw;
  }

  return moduleDescription;
}
