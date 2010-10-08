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
#include <QVariant>

// CTK includes
#include "ctkSlicerModuleReader.h"

// ----------------------------------------------------------------------------
void ctkSlicerModuleReader::update()
{

  QXmlSimpleReader xmlReader;
  QXmlInputSource *source = new QXmlInputSource(this->Device);
  
  ctkSlicerModuleHandler handler;
  handler.setModuleDescription(&this->ModuleDescription);
  xmlReader.setContentHandler(&handler);
  xmlReader.setErrorHandler(&handler);

  bool res = xmlReader.parse(source);

  if (!res)
    {
    throw std::runtime_error( tr("Parse error %1")
      .arg(handler.errorString()).toStdString() );
    }
}

// ----------------------------------------------------------------------------
ctkSlicerModuleHandler::ctkSlicerModuleHandler()
{
  this->ModuleDescription = 0;
  this->State.CurrentParameter = 0;
  this->State.CurrentGroup = 0;
  this->State.InExecutable = 0;
  this->State.InGroup = 0;
  this->State.InParameter = 0;
  this->ParamValidator = QRegExp("\\W");
}

// ----------------------------------------------------------------------------
void ctkSlicerModuleHandler::setModuleDescription(ctkModuleDescription* moduleDescription)
{
  this->ModuleDescription = moduleDescription;
}

// ----------------------------------------------------------------------------
ctkModuleDescription* ctkSlicerModuleHandler::moduleDescription()const
{
  return this->ModuleDescription;
}
// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::characters(const QString& ch)
{
  this->State.CurrentText = ch.trimmed();
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::startElement(const QString& namespaceURI, const QString& localName,
                                   const QString& name, const QXmlAttributes& atts)
{
  if (this->State.CurrentGroup == 0)
    {
    return this->startExecutableElement(namespaceURI, localName, name, atts);
    }
  else if (this->State.CurrentParameter == 0)
    {
    return this->startGroupElement(namespaceURI, localName, name, atts);
    }
  else
    {
    return this->startParameterElement(namespaceURI, localName, name, atts);
    }
  return false;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::endElement(const QString& namespaceURI,
                                 const QString& localName,
                                 const QString& qName)
{
  if (this->State.InParameter)
    {
    return this->endParameterElement(namespaceURI, localName, qName);
    }
  else if (this->State.InGroup)
    {
    return this->endGroupElement(namespaceURI, localName, qName);
    }
  else if (this->State.InExecutable)
    {
    return this->endExecutableElement(namespaceURI, localName, qName);
    }
  return false;
}
// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::startExecutableElement(const QString& namespaceURI,
                                             const QString& localName,
                                             const QString& name,
                                             const QXmlAttributes& atts)
{
  ++this->State.InExecutable;
  ctkModuleParameterGroup group;
  if (name == "parameters")
    {
    if (!atts.value("advanced").isEmpty())
      {
      group["Advanced"] = QVariant(atts.value("advanced")).toBool() ? "true" : "false";
      }
    this->State.CurrentGroup = new ctkModuleParameterGroup(group);
    }
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::startGroupElement(const QString& namespaceURI,
                                        const QString& localName,
                                        const QString& name,
                                        const QXmlAttributes& atts)
{
  ++this->State.InGroup;
  if (name == "label" || name == "description")
    {// not a parameter
    return true;
    }
  ctkModuleParameter param;
  param["Tag"] = name;
  bool multiple = QVariant(atts.value("multiple")).toBool(); //empty (not found) is false
  bool hidden = QVariant(atts.value("hidden")).toBool(); //empty (not found) is false
  
  if (name == "integer" || name == "integer-vector")
    {
    if (name == "integer-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<int>" : "int";
    param["ArgType"] = "int";
    param["StringToType"] = "atoi";
    }
  else if (name == "float" || name == "float-vector")
    {
    if (name == "integer-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<float>" : "float";
    param["ArgType"] = "float";
    param["StringToType"] = "atof";
    }
  else if (name == "double" || name == "double-vector")
    {
    if (name == "double-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<double>" : "double";
    param["ArgType"] = "double";
    param["StringToType"] = "atof";
    }
  else if (name == "string")
    {
    if (name == "string-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["StringToType"] = "";
    }
  else if (name == "boolean")
    {
    if (name == "boolean-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<bool>" : "bool";
    param["ArgType"] = "bool";
    param["StringToType"] = "";
    param["Hidden"] = hidden ? "true" : "false";
    }
  else if (name == "point" || name == "point-vector" ||
           name == "region" || name == "region-vector")
    {
    if (name == "point-vector" || name == "region-vector")
      {
      multiple = true;
      }
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<std::vector<float> >" : "std::vector<float>";
    param["ArgType"] = "float";
    param["StringToType"] = "atof";
    if (!atts.value("coordinateSystem").isEmpty())
      {
      param["CoordinateSystem"] = atts.value("coordinateSystem");
      }
    }
  else if (name == "string-enumeration")
    {
    param["CPPStyle"] = "std::string";
    }
  else if (name == "integer-enumeration")
    {
    param["CPPStyle"] = "int";
    }
  else if (name == "float-enumeration")
    {
    param["CPPStyle"] = "float";
    }
  else if (name == "double-enumeration")
    {
    param["CPPStyle"] = "double";
    }
  else if (name == "file")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPType"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = "scalar";
    if (!atts.value("fileExtensions").isEmpty())
      {
      param["FileExtensionsAsString"] = atts.value("fileExtensions");
      }
    }
  else if (name == "directory")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    }
  else if (name == "transform")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = atts.value("type").isEmpty() ? atts.value("type") : "unknown";
    if (!atts.value("fileExtensions").isEmpty())
      {
      param["FileExtensionsAsString"] = atts.value("fileExtensions");
      }
    if (!atts.value("reference").isEmpty())
      {
      param["Reference"] = atts.value("reference");
      }
    }
  else if (name == "image")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPStyle"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = (!atts.value("type").isEmpty()) ? atts.value("type") : "scalar";
    if (!atts.value("fileExtensions").isEmpty())
      {
      param["FileExtensionsAsString"] = atts.value("fileExtensions");
      }
    param["Hidden"] = hidden ? "true" : "false";
    if (!atts.value("reference").isEmpty())
      {
      param["Reference"] = atts.value("reference");
      }
    }
  else if (name == "geometry")
    {
    bool aggregate = QVariant(atts.value("aggregate")).toBool();
    param["Multiple"] = multiple ? "true" : "false";
    param["Aggregate"] = aggregate ? "true" : "false";
    param["CPPType"] = (multiple && !aggregate) ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = (!atts.value("type").isEmpty())? atts.value("type").isEmpty() : "scalar";
    }
  else if (name == "table")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPType"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = (!atts.value("type").isEmpty())? atts.value("type").isEmpty() : "scalar";
    if (!atts.value("reference").isEmpty())
      {
      param["Reference"] = atts.value("reference");
      }
    if (!atts.value("fileExtensions").isEmpty())
      {
      param["FileExtensionsAsString"] = atts.value("fileExtensions");
      }
    }
  else if (name == "measurement")
    {
    param["Multiple"] = multiple ? "true" : "false";
    param["CPPType"] = multiple ? "std::vector<std::string>" : "std::string";
    param["ArgType"] = "std::string";
    param["Type"] = (!atts.value("type").isEmpty())? atts.value("type").isEmpty() : "scalar";
    param["Hidden"] = hidden ? "true" : "false";
    if (!atts.value("reference").isEmpty())
      {
      param["Reference"] = atts.value("reference");
      }
    if (!atts.value("fileExtensions").isEmpty())
      {
      param["FileExtensionsAsString"] = atts.value("fileExtensions");
      }
    }
  this->State.CurrentParameter = new ctkModuleParameter(param);
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::startParameterElement(const QString& namespaceURI, const QString& localName,
                                            const QString& name, const QXmlAttributes& atts)
{
  ++this->State.InParameter;
  ctkModuleParameter& param = *this->State.CurrentParameter;
  if (name == "flag")
    {
    if (!atts.value("alias").isEmpty())
      {
      param["FlagAliasesAsString"] = atts.value("alias");
      }
    if (!atts.value("deprecatedalias").isEmpty())
      {
      param["DeprecatedFlagAliasesAsString"] = atts.value("deprecatedalias");
      }
    }
  else if (name == "longflag")
    {
    if (!atts.value("alias").isEmpty())
      {
      param["LongFlagAliasesAsString"] = atts.value("alias");
      }
    if (!atts.value("deprecatedalias").isEmpty())
      {
      param["DeprecatedLongFlagAliasesAsString"] = atts.value("deprecatedalias");
      }
    }
  else if (name == "constraints")
    {
    param["Constraints"] = "true";
    }
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::endExecutableElement(const QString& namespaceURI,
                                           const QString& localName,
                                           const QString& name)
{
  --this->State.InExecutable;
  ctkModuleDescription& module= *this->ModuleDescription;
  if (name == "parameters")
    {
    Q_ASSERT(this->State.CurrentGroup);
    this->ModuleDescription->addParameterGroup(this->State.CurrentGroup);
    this->State.CurrentGroup = 0;
    }
  else if (name == "category")
    {
    module["Category"] = this->State.CurrentText;
    }
  else if (name == "index")
    {
    module["Index"] = this->State.CurrentText;
    }
  else if (name == "title")
    {
    module["Title"] = this->State.CurrentText;
    }
  else if (name == "version")
    {
    module["Version"] = this->State.CurrentText;
    }
  else if (name == "documentation-url")
    {
    module["DocumentationUrl"] = this->State.CurrentText;
    }
  else if (name == "license")
    {
    module["License"] = this->State.CurrentText;
    }
  else if (name == "acknowledgements")
    {
    module["License"] = this->State.CurrentText;
    }
  else if (name == "contributor")
    {
    module["Contributor"] = this->State.CurrentText;
    }
  else if (name == "location")
    {
    module["Location"] = this->State.CurrentText;
    }
  else if (name == "description")
    {
    module["Description"] = this->State.CurrentText.replace('\"','\'');
    }
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::endGroupElement(const QString& namespaceURI,
                                      const QString& localName,
                                      const QString& name)
{
  --this->State.InGroup;
  Q_ASSERT(this->State.CurrentGroup);
  ctkModuleParameterGroup& group = *this->State.CurrentGroup;
  if (name == "label")
    {
    group["Label"] = this->State.CurrentText;
    return true;
    }
  else if (name == "description")
    {
    group["Description"] = this->State.CurrentText.replace('\"', '\'');
    return true;
    }
  Q_ASSERT(this->State.CurrentParameter);
  this->State.CurrentGroup->addParameter(this->State.CurrentParameter);
  this->State.CurrentParameter = 0;
  return true;
}

// ----------------------------------------------------------------------------
bool ctkSlicerModuleHandler::endParameterElement(const QString& namespaceURI, const QString& localName,
                                          const QString& name)
{
  --this->State.InParameter;
  bool res = true;
  ctkModuleParameter& parameter = *this->State.CurrentParameter;
  if (name == "flag")
    {
    QString flag = this->State.CurrentText;
    flag.remove(0, flag.indexOf('-') + 1);
    res = (flag.size() == 1);
    if (!res)
      {
      qWarning() << "Invalid flag" << flag;
      }
    parameter["Flag"] = flag;
    }
  else if (name == "longflag")
    {
    QString flag = this->State.CurrentText;
    flag.remove(0, flag.indexOf("--") + 2);
    res = (flag.size() != 0 && this->ParamValidator.indexIn(flag) == -1);
    if (!res)
      {
      qWarning() << "Invalid flag" << flag;
      }
    parameter["LongFlag"] = flag;
    parameter["Name"] = parameter.value("Name", flag);
    }
  else if (name == "name")
    {
    QString paramName = this->State.CurrentText;
    if (this->ParamValidator.indexIn(paramName) != -1)
      {
      res = false;
      qWarning() << "Invalid name" << paramName;
      }
    parameter["Name"] = paramName;
    }
  else if (name == "label")
    {
    parameter["Label"] = this->State.CurrentText;
    }
  else if (name == "element")
    {
    parameter.insertMulti("Element", this->State.CurrentText);
    }
  else if (name == "default")
    {
    parameter["Default"] = this->State.CurrentText;
    }
  else if (name == "channel")
    {
    parameter["Channel"] = this->State.CurrentText;
    }
  else if (name == "index")
    {
    // TODO make sure Flag and LongFlag are empty
    parameter["Index"] = this->State.CurrentText;
    }
  else if (name == "minimum")
    {
    parameter["Minimum"] = this->State.CurrentText;
    }
  else if (name == "maximum")
    {
    parameter["Maximum"] = this->State.CurrentText;
    }
  else if (name == "step")
    {
    parameter["Step"] = this->State.CurrentText;
    }
  return res;
}
