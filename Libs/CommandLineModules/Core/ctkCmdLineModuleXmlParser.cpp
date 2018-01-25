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
#include "ctkCmdLineModuleXmlParser_p.h"
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleDescription_p.h"
#include "ctkCmdLineModuleParameterGroup.h"
#include "ctkCmdLineModuleParameterGroup_p.h"
#include "ctkCmdLineModuleParameterParsers_p.h"

#include "ctkCmdLineModuleXmlException.h"

// STD includes
#include <stdexcept>


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
  _paramParsers["directory"] = new ctkCmdLineModuleMultipleParameterParser; // type="multipleType"/>
  _paramParsers["image"] = new ctkCmdLineModuleImageParameterParser; // type="imageType"/>
  _paramParsers["geometry"] = new ctkCmdLineModuleGeometryParameterParser; // type="geometryType"/>
}

// ----------------------------------------------------------------------------
ctkCmdLineModuleXmlParser::~ctkCmdLineModuleXmlParser()
{
  qDeleteAll(_paramParsers.values());
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
    throw ctkCmdLineModuleXmlException(msg.arg(_xmlReader.lineNumber()).arg(_xmlReader.columnNumber())
                                       .arg(_xmlReader.errorString()));
  }
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleXmlParser::handleExecutableElement()
{
  while(_xmlReader.readNextStartElement())
  {
    QStringRef name = _xmlReader.name();

    if (compare(name, "category", Qt::CaseInsensitive) == 0)
    {
      _md->d->Category = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "title", Qt::CaseInsensitive) == 0)
    {
      _md->d->Title = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "version", Qt::CaseInsensitive) == 0)
    {
      _md->d->Version = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "documentation-url", Qt::CaseInsensitive) == 0)
    {
      _md->d->DocumentationURL = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "license", Qt::CaseInsensitive) == 0)
    {
      _md->d->License = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "acknowledgements", Qt::CaseInsensitive) == 0)
    {
      _md->d->Acknowledgements = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "contributor", Qt::CaseInsensitive) == 0)
    {
      _md->d->Contributor = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "description", Qt::CaseInsensitive) == 0)
    {
      _md->d->Description = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "parameters", Qt::CaseInsensitive) == 0)
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
  ctkCmdLineModuleParameterGroup group;

  group.d->Advanced = parseBooleanAttribute(_xmlReader.attributes().value("advanced"));

  while(_xmlReader.readNextStartElement())
  {
    QStringRef name = _xmlReader.name();

    if (compare(name, "label", Qt::CaseInsensitive) == 0)
    {
      group.d->Label = _xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "description", Qt::CaseInsensitive) == 0)
    {
      group.d->Description = _xmlReader.readElementText().trimmed();
    }
    else
    {
      try
      {
        ctkCmdLineModuleParameter parameter = this->handleParameterElement();
        group.d->Parameters.push_back(parameter);
      }
      catch (const ctkException& e)
      {
        // Just print the exception and continue
        qCritical() << e;
      }
    }
  }

  _md->d->ParameterGroups.push_back(group);
}

// ----------------------------------------------------------------------------
ctkCmdLineModuleParameter ctkCmdLineModuleXmlParser::handleParameterElement()
{
  QString paramTag = _xmlReader.name().toString().toLower();
  ctkCmdLineModuleParameterParser* paramParser = _paramParsers[paramTag];
  if (paramParser == 0)
  {
    _xmlReader.skipCurrentElement();
    QString msg = "No parser for element \"%1\" available, line %2, column %3";
    throw ctkIllegalStateException(msg.arg(paramTag).arg(_xmlReader.lineNumber()).arg(_xmlReader.columnNumber()));
  }
  else
  {
    ctkCmdLineModuleParameter moduleParam = paramParser->parse(_xmlReader);
    moduleParam.d->Tag = paramTag;
    return moduleParam;
  }
}
