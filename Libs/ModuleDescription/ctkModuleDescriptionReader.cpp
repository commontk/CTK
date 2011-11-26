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
#include "ctkModuleDescription.h"

// STD includes
#include <stdexcept>

class ctkModuleDescriptionParser
{
public:

  ctkModuleDescriptionParser(QIODevice* device, ctkModuleDescription* md);

  void validate();
  void doParse();

  void handleExecutableElement();
  void handleParametersElement();
  void handleParameterElement();

private:

  QIODevice* const _device;
  ctkModuleDescription* _md;
  QXmlStreamReader _xmlReader;
};

namespace {
static const QString EXECUTABLE = "executable";
}

// ----------------------------------------------------------------------------
ctkModuleDescriptionParser::ctkModuleDescriptionParser(QIODevice* device,
                                                       ctkModuleDescription* md)
  : _device(device), _md(md)
{}

// ----------------------------------------------------------------------------
void ctkModuleDescriptionParser::validate()
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
  schema.load(QUrl::fromLocalFile(":ctkModuleDescription.xsd"));

  bool res = schema.isValid();
  if (!res)
  {
    throw std::runtime_error(std::string("Invalid Schema ") +
                             errorHandler.statusMessage().toStdString());
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
void ctkModuleDescriptionParser::doParse()
{
  _xmlReader.clear();
  _xmlReader.setDevice(_device);
  while(!_xmlReader.hasError() && _xmlReader.readNextStartElement())
  {
    QStringRef name = _xmlReader.name();
    if (name.compare(EXECUTABLE, Qt::CaseInsensitive) == 0)
    {
      this->handleExecutableElement();
    }
  }

  if (_xmlReader.hasError() &&
      !(_xmlReader.atEnd() && _xmlReader.error() == QXmlStreamReader::PrematureEndOfDocumentError))
  {
    throw std::runtime_error(std::string("Error parsing XML description: ") +
                             _xmlReader.errorString().toStdString());
  }
}

// ----------------------------------------------------------------------------
void ctkModuleDescriptionParser::handleExecutableElement()
{

}

// ----------------------------------------------------------------------------
void ctkModuleDescriptionParser::handleParametersElement()
{

}

// ----------------------------------------------------------------------------
void ctkModuleDescriptionParser::handleParameterElement()
{

}



// ===============================================================================

void slicerParser()
{

if (name == "parameters")
{

  group = new ModuleParameterGroup;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 && (strcmp(attrs[0], "advanced") == 0))
  {
    group->SetAdvanced(attrs[1]);
  }
}
else if (name == "integer")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 &&
      (strcmp(attrs[0], "multiple") == 0) &&
      (strcmp(attrs[1], "true") == 0))
  {
    parameter->SetMultiple(attrs[1]);
    parameter->SetCPPType("std::vector<int>");
    parameter->SetArgType("int");
    parameter->SetStringToType("atoi");      }
  else
  {
    parameter->SetCPPType("int");
  }
}
else if (name == "float")
{
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 &&
      (strcmp(attrs[0], "multiple") == 0) &&
      (strcmp(attrs[1], "true") == 0))
  {
    parameter->SetMultiple(attrs[1]);
    parameter->SetCPPType("std::vector<float>");
    parameter->SetArgType("float");
    parameter->SetStringToType("atof");
  }
  else
  {
    parameter->SetCPPType("float");
  }
  parameter->SetTag(name);
}
else if (name == "double")
{
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 &&
      (strcmp(attrs[0], "multiple") == 0) &&
      (strcmp(attrs[1], "true") == 0))
  {
    parameter->SetMultiple(attrs[1]);
    parameter->SetCPPType("std::vector<double>");
    parameter->SetArgType("double");
    parameter->SetStringToType("atof");
  }
  else
  {
    parameter->SetCPPType("double");
  }
  parameter->SetTag(name);
}
else if (name == "string")
{
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 &&
      (strcmp(attrs[0], "multiple") == 0) &&
      (strcmp(attrs[1], "true") == 0))
  {
    parameter->SetMultiple(attrs[1]);
    parameter->SetCPPType("std::vector<std::string>");
    parameter->SetArgType("std::string");
    parameter->SetStringToType("");      }
  else
  {
    parameter->SetCPPType("std::string");
  }
  parameter->SetTag(name);
}
else if (name == "boolean")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("bool");

  // Parse attribute pairs
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetHidden(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"hidden\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
  }
}
else if (name == "integer-vector")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("std::vector<int>");
  parameter->SetArgType("int");
  parameter->SetStringToType("atoi");
}
else if (name == "float-vector")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("std::vector<float>");
  parameter->SetArgType("float");
  parameter->SetStringToType("atof");
}
else if (name == "string-vector")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("std::vector<std::string>");
  parameter->SetArgType("std::string");
  parameter->SetStringToType("");
}
else if (name == "double-vector")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("std::vector<double>");
  parameter->SetArgType("double");
  parameter->SetStringToType("atof");
}
else if (name == "point")
{
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  // Parse attribute pairs
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        parameter->SetCPPType("std::vector<std::vector<float> >");
        parameter->SetArgType("float");
        parameter->SetStringToType("atof");
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "coordinateSystem") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "ijk") == 0) ||
          (strcmp(attrs[2*attr+1], "lps") == 0) ||
          (strcmp(attrs[2*attr+1], "ras") == 0))
      {
        parameter->SetCoordinateSystem(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid coordinate system. Only \"ijk\", \"lps\" and \"ras\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: " + std::string(attrs[2*attr]) + " is not a valid attribute for the tag" + name);
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  if (parameter->GetMultiple() != "true")
  {
    parameter->SetCPPType("std::vector<float>");
    parameter->SetArgType("float");
    parameter->SetStringToType("atof");
  }
  parameter->SetTag(name);
}
else if (name == "region")
{
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  // Parse attribute pairs
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        parameter->SetCPPType("std::vector<std::vector<float> >");
        parameter->SetArgType("float");
        parameter->SetStringToType("atof");
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "coordinateSystem") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "ijk") == 0) ||
          (strcmp(attrs[2*attr+1], "lps") == 0) ||
          (strcmp(attrs[2*attr+1], "ras") == 0))
      {
        parameter->SetCoordinateSystem(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid coordinate system. Only \"ijk\", \"lps\" and \"ras\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: " + std::string(attrs[2*attr]) + " is not a valid attribute for the tag" + name);
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  if (parameter->GetMultiple() != "true")
  {
    parameter->SetCPPType("std::vector<float>");
    parameter->SetArgType("float");
    parameter->SetStringToType("atof");
  }
  parameter->SetTag(name);
}
else if (name == "string-enumeration")
{
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("std::string");
  // Parse attribute pairs
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    bool res = true;
    if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      res = ModuleDescriptionParser::processHiddenAttribute(attrs[2*attr+1], parameter, ps);
    }
    if (!res)
    {
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
}
else if (name == "integer-enumeration")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("int");
  // Parse attribute pairs
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    bool res = true;
    if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      res = ModuleDescriptionParser::processHiddenAttribute(attrs[2*attr+1], parameter, ps);
    }
    if (!res)
    {
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
}
else if (name == "float-enumeration")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("float");
  // Parse attribute pairs
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    bool res = true;
    if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      res = ModuleDescriptionParser::processHiddenAttribute(attrs[2*attr+1], parameter, ps);
    }
    if (!res)
    {
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
}
else if (name == "double-enumeration")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  parameter->SetTag(name);
  parameter->SetCPPType("double");
  // Parse attribute pairs
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    bool res = true;
    if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      res = ModuleDescriptionParser::processHiddenAttribute(attrs[2*attr+1], parameter, ps);
    }
    if (!res)
    {
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
}
else if (name == "file")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("scalar");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        if (strcmp(attrs[2*attr+1], "true") == 0)
        {
          parameter->SetCPPType("std::vector<std::string>");
          parameter->SetArgType("std::string");
        }
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\" and \"fileExtensions\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
}
else if (name == "directory")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount == 2 &&
      (strcmp(attrs[0], "multiple") == 0) &&
      (strcmp(attrs[1], "true") == 0))
  {
    parameter->SetMultiple(attrs[1]);
    parameter->SetCPPType("std::vector<std::string>");
    parameter->SetArgType("std::string");
  }
  else
  {
    parameter->SetCPPType("std::string");
  }
  parameter->SetTag(name);
}
else if (name == "transform")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("unknown");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        if (strcmp(attrs[2*attr+1], "true") == 0)
        {
          parameter->SetCPPType("std::vector<std::string>");
          parameter->SetArgType("std::string");
        }
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "type") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "linear") == 0) ||
          (strcmp(attrs[2*attr+1], "nonlinear") == 0) ||
          (strcmp(attrs[2*attr+1], "bspline") == 0))
      {
        parameter->SetType(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid value for the attribute \"" + "type" + "\". Only \"linear\" and \"nonlinear\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else if ((strcmp(attrs[2*attr], "reference") == 0))
    {
      parameter->SetReference(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\", \"fileExtensions\" and \"type\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
}
else if (name == "image")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("scalar");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        if (strcmp(attrs[2*attr+1], "true") == 0)
        {
          parameter->SetCPPType("std::vector<std::string>");
          parameter->SetArgType("std::string");
        }
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "type") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "any") == 0) ||
          (strcmp(attrs[2*attr+1], "scalar") == 0) ||
          (strcmp(attrs[2*attr+1], "label") == 0) ||
          (strcmp(attrs[2*attr+1], "tensor") == 0) ||
          (strcmp(attrs[2*attr+1], "diffusion-weighted") == 0) ||
          (strcmp(attrs[2*attr+1], "vector") == 0))
      {
        parameter->SetType(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid value for the attribute \"" + "type" + "\". Only \"scalar\", \"label\" , \"tensor\", \"diffusion-weighted\", \"vector\" and \"any\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetHidden(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"hidden\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "reference") == 0))
    {
      parameter->SetReference(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\", \"fileExtensions\", \"type\", \"hidden\" and \"reference\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
}
else if (name == "geometry")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("scalar");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "aggregate") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetAggregate(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"aggregate\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }      else if ((strcmp(attrs[2*attr], "type") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "fiberbundle") == 0) ||
          (strcmp(attrs[2*attr+1], "model") == 0))
      {
        parameter->SetType(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid value for the attribute \"" + "type" + "\". Only \"fiberbundle\", \"model\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\", \"type\", and \"fileExtensions\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
  if (parameter->GetMultiple() == "true"
      && parameter->GetAggregate() != "true")
  {
    parameter->SetCPPType("std::vector<std::string>");
    parameter->SetArgType("std::string");
  }
}
else if (name == "table")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("scalar");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        if (strcmp(attrs[2*attr+1], "true") == 0)
        {
          parameter->SetCPPType("std::vector<std::string>");
          parameter->SetArgType("std::string");
        }
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "reference") == 0))
    {
      parameter->SetReference(attrs[2*attr+1]);
    }
    else if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetHidden(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"hidden\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "type") == 0))
    {
      // Need to add other tables with context (similar to color
      // tables) as well as add general CSV files
      if ((strcmp(attrs[2*attr+1], "color") == 0))
      {
        parameter->SetType(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid value for the attribute \"" + "type" + "\". Only \"color\" is currently accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\", \"hidden\", \"reference\", \"type\", and \"fileExtensions\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
}
else if (name == "measurement")
{
  if (!group || (ps->OpenTags.top() != "parameters"))
  {
    std::string error("ModuleDescriptionParser Error: <" + name + "> can only be used inside <parameters> but was found inside <" + ps->OpenTags.top() + ">");
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
    ps->OpenTags.push(name);
    delete parameter;
    return;
  }
  parameter = new ModuleParameter;
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);

  // Parse attribute pairs
  parameter->SetCPPType("std::string");
  parameter->SetType("scalar");
  for (int attr=0; attr < (attrCount / 2); attr++)
  {
    if ((strcmp(attrs[2*attr], "multiple") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetMultiple(attrs[2*attr+1]);
        if (strcmp(attrs[2*attr+1], "true") == 0)
        {
          parameter->SetCPPType("std::vector<std::string>");
          parameter->SetArgType("std::string");
        }
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"multiple\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "reference") == 0))
    {
      parameter->SetReference(attrs[2*attr+1]);
    }
    else if ((strcmp(attrs[2*attr], "hidden") == 0))
    {
      if ((strcmp(attrs[2*attr+1], "true") == 0) ||
          (strcmp(attrs[2*attr+1], "false") == 0))
      {
        parameter->SetHidden(attrs[2*attr+1]);
      }
      else
      {
        std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid argument for the attribute \"hidden\". Only \"true\" and \"false\" are accepted.");
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        ps->OpenTags.push(name);
        delete parameter;
        return;
      }
    }
    else if ((strcmp(attrs[2*attr], "type") == 0))
    {
      // "type" will refer to the error models (regression,
      // orthogonal regression, ...). We'll define these later. When
      // we do, put the error handling back in.
      //         if ((strcmp(attrs[2*attr+1], "some_error_type") == 0))
      //           {
      parameter->SetType(attrs[2*attr+1]);
      //           }
      //         else
      //           {
      //           std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr+1]) + "\" is not a valid value for the attribute \"" + "type" + "\". Only \"color\" is currently accepted.");
      //           if (ps->ErrorDescription.size() == 0)
      //             {
      //             ps->ErrorDescription = error;
      //             ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      //             ps->Error = true;
      //             }
      //           ps->OpenTags.push(name);
      //           delete parameter;
      //           return;
      //           }
    }
    else if ((strcmp(attrs[2*attr], "fileExtensions") == 0))
    {
      parameter->SetFileExtensionsAsString(attrs[2*attr+1]);
    }
    else
    {
      std::string error("ModuleDescriptionParser Error: \"" + std::string(attrs[2*attr]) + "\" is not a valid attribute for \"" + name + "\". Only \"multiple\", \"hidden\", \"reference\", \"type\", and \"fileExtensions\" are accepted.");
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      ps->OpenTags.push(name);
      delete parameter;
      return;
    }
  }
  parameter->SetTag(name);
}
else if (parameter && (name == "flag"))
{
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount > 0)
  {
    for (int i=0; i < attrCount/2; ++i)
    {
      if (strcmp(attrs[2*i], "alias") == 0)
      {
        parameter->SetFlagAliasesAsString(attrs[2*i+1]);
      }
      else if (strcmp(attrs[2*i], "deprecatedalias") == 0)
      {
        parameter->SetDeprecatedFlagAliasesAsString(attrs[2*i+1]);
      }
    }
  }
}
else if (parameter && (name == "longflag"))
{
  int attrCount = XML_GetSpecifiedAttributeCount(ps->Parser);
  if (attrCount > 0)
  {
    for (int i=0; i < attrCount/2; ++i)
    {
      if (strcmp(attrs[2*i], "alias") == 0)
      {
        parameter->SetLongFlagAliasesAsString(attrs[2*i+1]);
      }
      else if (strcmp(attrs[2*i], "deprecatedalias") == 0)
      {
        parameter->SetDeprecatedLongFlagAliasesAsString(attrs[2*i+1]);
      }
    }
  }
}
ps->CurrentParameter = parameter;
ps->CurrentGroup = group;
ps->OpenTags.push(name);

}

void
endElement(void *userData, const char *element)
{
  ParserState *ps = reinterpret_cast<ParserState *>(userData);
  ModuleParameter *parameter = ps->CurrentParameter;
  ModuleParameterGroup *group = ps->CurrentGroup;
  std::string name(element);

  if (name == "parameters" && ps->Depth == 1)
  {
    ps->CurrentDescription.AddParameterGroup(*ps->CurrentGroup);
    if (ps->CurrentGroup)
    {
      delete ps->CurrentGroup;
    }
    ps->CurrentGroup = 0;
    ps->CurrentParameter = 0;
  }
  else if (name == "parameters" && ps->Depth != 1)
  {
    ps->CurrentGroup = 0;
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "integer"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "float"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "double"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "string"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name =="boolean"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "file"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "directory"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "transform"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "image"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "geometry"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "table"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "measurement"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "integer-vector"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "float-vector"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "string-vector"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "double-vector"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "point"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "region"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "string-enumeration"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "integer-enumeration"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "float-enumeration"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (group && parameter && (name == "double-enumeration"))
  {
    ps->CurrentGroup->AddParameter(*parameter);
    if (ps->CurrentParameter)
    {
      delete ps->CurrentParameter;
    }
    ps->CurrentParameter = 0;
  }
  else if (parameter && (name == "flag"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    trimLeading(temp, "-");
    if (temp.size() > 1)
    {
      std::string error("ModuleDescriptionParser Error: <"
                        + name
                        + std::string("> can only contain one character. \"")
                        + temp
                        + std::string("\" has more than one character."));
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      if (!ps->OpenTags.empty())
      {
        ps->OpenTags.pop();
      }
      ps->Depth--;
      return;
    }
    else
    {
      if (!parameter->GetIndex().empty())
      {
        std::string error("ModuleDescriptionParser Error: <"
                          + std::string(name)
                          + "> cannot be specified because an index has been specified for this parameter."
                          + std::string("\""));
        if (ps->ErrorDescription.size() == 0)
        {
          ps->ErrorDescription = error;
          ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
          ps->Error = true;
        }
        if (!ps->OpenTags.empty())
        {
          ps->OpenTags.pop();
          ps->Depth--;
        }
        return;
      }
      parameter->SetFlag(temp);
    }
  }
  else if (parameter && (name == "longflag"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    trimLeading(temp, "-");
    if (!validVariable(temp))
    {
      std::string error("ModuleDescriptionParser Error: <"
                        + std::string(name)
                        + "> can only contain letters, numbers and underscores and must start with a _ or letter. The offending name is \""
                        + temp
                        + std::string("\""));
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      if (!ps->OpenTags.empty())
      {
        ps->OpenTags.pop();
        ps->Depth--;
      }
      return;
    }
    if (!parameter->GetIndex().empty())
    {
      std::string error("ModuleDescriptionParser Error: <"
                        + std::string(name)
                        + "> cannot be specified because an index has been specified for this parameter."
                        + std::string("\""));
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      if (!ps->OpenTags.empty())
      {
        ps->OpenTags.pop();
        ps->Depth--;
      }
      return;
    }
    parameter->SetLongFlag(temp);
    if (parameter->GetName().empty())
    {
      parameter->SetName(temp);
    }
  }
  else if (parameter && (name == "name"))
  {
    std::string temp = std::string(ps->LastData[ps->Depth]);
    trimLeadingAndTrailing(temp);
    if (!validVariable(temp))
    {
      std::string error("ModuleDescriptionParser Error: <"
                        + std::string(name)
                        + "> can only contain letters, numbers and underscores and must start with an _ or letter. The offending name is \""
                        + temp
                        + std::string("\""));
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      if (!ps->OpenTags.empty())
      {
        ps->OpenTags.pop();
        ps->Depth--;
      }
      return;
    }
    parameter->SetName(temp);
  }
  else if ((group || parameter) && (name == "label"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    if (group && !parameter)
    {
      group->SetLabel(temp);
    }
    else
    {
      parameter->SetLabel(temp);
    }
  }
  else if (name == "category")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetCategory(temp);
  }
  else if (!parameter && (name == "index"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetIndex(temp);
  }
  else if (name == "title")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetTitle(temp);
  }
  else if (name == "version")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetVersion(temp);
  }
  else if (name == "documentation-url")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetDocumentationURL(temp);
  }
  else if (name == "license")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetLicense(temp);
  }
  else if (name == "acknowledgements")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetAcknowledgements(temp);
  }
  else if (name == "contributor")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetContributor(temp);
  }
  else if (name == "location")
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetLocation(temp);
  }
  else if (name ==  "description")
  {
    std::string temp = ps->LastData[ps->Depth];
    replaceSubWithSub(temp, "\"", "'");
    //replaceSubWithSub(temp, "\n", " ");
    trimLeadingAndTrailing(temp);
    if (!group && !parameter)
    {
      ps->CurrentDescription.SetDescription(temp);
    }
    else if (group && !parameter)
    {
      group->SetDescription(temp);
    }
    else
    {
      parameter->SetDescription(temp);
    }
  }
  else if (parameter && (name == "element"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->GetElements().push_back(temp);
  }
  else if (parameter && (name == "default"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetDefault(temp);
  }
  else if (parameter && (name == "channel"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetChannel(temp);
  }
  else if (parameter && (name == "index"))
  {
    if (!parameter->GetLongFlag().empty() || !parameter->GetFlag().empty() )
    {
      std::string error("ModuleDescriptionParser Error: <"
                        + std::string(name)
                        + "> cannot be specified because a <longflag> and/or <flag> has been specified for this parameter."
                        + std::string("\""));
      if (ps->ErrorDescription.size() == 0)
      {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
      }
      if (!ps->OpenTags.empty())
      {
        ps->OpenTags.pop();
        ps->Depth--;
      }
      return;
    }
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetIndex(temp);
  }
  else if (parameter && (name == "constraints"))
  {
    parameter->SetConstraints(name);
  }
  else if (parameter && (name == "minimum"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetMinimum(temp);
  }
  else if (parameter && (name == "maximum"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetMaximum(temp);
  }
  else if (parameter && (name == "step"))
  {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    parameter->SetStep(temp);
  }
  else if(name != "executable")
  {
    std::string error("ModuleDescriptionParser Error: Unrecognized element <" + name + std::string("> was found."));
    if (ps->ErrorDescription.size() == 0)
    {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
    }
  }

}

  // ===============================================================================



  // ----------------------------------------------------------------------------
  ctkModuleDescription* ctkModuleDescription::parse(QIODevice* device)
  {
    ctkModuleDescription* moduleDescription = new ctkModuleDescription();
    ctkModuleDescriptionParser parser(device, moduleDescription);

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
