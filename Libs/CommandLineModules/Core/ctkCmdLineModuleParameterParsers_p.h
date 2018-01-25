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

#ifndef CTKCMDLINEMODULEPARAMETERPARSERS_P_H
#define CTKCMDLINEMODULEPARAMETERPARSERS_P_H

#include <QXmlStreamReader>

#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameter_p.h"

namespace {

static int compare(const QStringRef& value1, const char* value2, Qt::CaseSensitivity cs = Qt::CaseSensitive)
{
  return value1.compare(QLatin1String(value2), cs);
}

static bool parseBooleanAttribute(const QStringRef& attrValue)
{
  if (compare(attrValue, "true", Qt::CaseInsensitive) == 0 ||
      compare(attrValue, "1") == 0)
  {
    return true;
  }
  return false;
}

}

struct ctkCmdLineModuleParameterParser
{
public:

  virtual ~ctkCmdLineModuleParameterParser() {}

  virtual ctkCmdLineModuleParameter parse(QXmlStreamReader &xmlReader)
  {
    ctkCmdLineModuleParameter moduleParam = this->createModuleParameter();
    this->handleAttributes(moduleParam.d.data(), xmlReader);

    while(xmlReader.readNextStartElement())
    {
      this->handleSubElement(moduleParam.d.data(), xmlReader);
    }

    return moduleParam;
  }

protected:

  virtual ctkCmdLineModuleParameter createModuleParameter()
  {
    return ctkCmdLineModuleParameter();
  }

  virtual void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    // handle common attributes
    moduleParamPrivate->Hidden = parseBooleanAttribute(xmlReader.attributes().value("hidden"));
  }

  virtual bool handleSubElement(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    // handle common sub-elements

    QStringRef name = xmlReader.name();

    if (compare(name, "name", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Name = xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "description", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Description = xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "label", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Label = xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "default", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Default = xmlReader.readElementText().trimmed();
    }
    else if (compare(name, "flag", Qt::CaseInsensitive) == 0)
    {
      QString flag = xmlReader.readElementText().trimmed();
      if (flag.startsWith('-')) flag = flag.remove(0, 1);
      moduleParamPrivate->Flag = flag;
      moduleParamPrivate->FlagAliasesAsString = xmlReader.attributes().value("alias").toString();
      moduleParamPrivate->DeprecatedFlagAliasesAsString = xmlReader.attributes().value("deprecatedalias").toString();
    }
    else if (compare(name, "longflag", Qt::CaseInsensitive) == 0)
    {
      QString longFlag = xmlReader.readElementText().trimmed();
      if (longFlag.startsWith('-')) longFlag = longFlag.remove(0, 1);
      moduleParamPrivate->LongFlag = longFlag;
      moduleParamPrivate->LongFlagAliasesAsString = xmlReader.attributes().value("alias").toString();
      moduleParamPrivate->DeprecatedLongFlagAliasesAsString = xmlReader.attributes().value("deprecatedalias").toString();
    }
    else if (compare(name, "index", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Index = xmlReader.readElementText().toInt();
    }
    else if (compare(name, "channel", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Channel = xmlReader.readElementText().trimmed();
    }
    else
    {
      xmlReader.skipCurrentElement();
      return false;
    }

    return true;
  }

  bool handleConstraintsElement(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    moduleParamPrivate->Constraints = true;
    while(xmlReader.readNextStartElement())
    {
      QStringRef constraintElem = xmlReader.name();
      if (compare(constraintElem, "minimum", Qt::CaseInsensitive) == 0)
      {
        moduleParamPrivate->Minimum = xmlReader.readElementText().trimmed();
      }
      else if (compare(constraintElem, "maximum", Qt::CaseInsensitive) == 0)
      {
        moduleParamPrivate->Maximum = xmlReader.readElementText().trimmed();
      }
      else if (compare(constraintElem, "step", Qt::CaseInsensitive) == 0)
      {
        moduleParamPrivate->Step = xmlReader.readElementText().trimmed();
      }
      else
      {
        xmlReader.skipCurrentElement();
        return false;
      }
    }
    return true;
  }
};

class ctkCmdLineModuleMultipleParameterParser : public ctkCmdLineModuleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleParameterParser::handleAttributes(moduleParamPrivate, xmlReader);
    moduleParamPrivate->Hidden = parseBooleanAttribute(xmlReader.attributes().value("multiple"));
  }
};

class ctkCmdLineModuleScalarVectorParameterParser : public ctkCmdLineModuleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (compare(name, "constraints", Qt::CaseInsensitive) == 0)
    {
      return handleConstraintsElement(moduleParamPrivate, xmlReader);
    }
    else
    {
      return ctkCmdLineModuleParameterParser::handleSubElement(moduleParamPrivate, xmlReader);
    }
  }
};

class ctkCmdLineModuleScalarParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (compare(name, "constraints", Qt::CaseInsensitive) == 0)
    {
      return handleConstraintsElement(moduleParamPrivate, xmlReader);
    }
    else
    {
      return ctkCmdLineModuleMultipleParameterParser::handleSubElement(moduleParamPrivate, xmlReader);
    }
  }
};

class ctkCmdLineModuleEnumerationParameterParser : public ctkCmdLineModuleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (compare(name, "element", Qt::CaseInsensitive) == 0)
    {
      moduleParamPrivate->Elements.push_back(xmlReader.readElementText().trimmed());
      return true;
    }
    else
    {
      return ctkCmdLineModuleParameterParser::handleSubElement(moduleParamPrivate, xmlReader);
    }
  }
};

class ctkCmdLineModulePointParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParamPrivate, xmlReader);
    moduleParamPrivate->CoordinateSystem = xmlReader.attributes().value("coordinateSystem").toString().trimmed();
  }
};

class ctkCmdLineModuleFileParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParamPrivate, xmlReader);
    moduleParamPrivate->FileExtensionsAsString =xmlReader.attributes().value("fileExtensions").toString().trimmed();
  }
};

class ctkCmdLineModuleGeometryParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParamPrivate, xmlReader);
    moduleParamPrivate->setFileExtensionsAsString(xmlReader.attributes().value("fileExtensions").toString().trimmed());
    moduleParamPrivate->Type = xmlReader.attributes().value("type").toString().trimmed();
  }
};

class ctkCmdLineModuleImageParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameterPrivate* moduleParamPrivate, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParamPrivate, xmlReader);
    moduleParamPrivate->setFileExtensionsAsString(xmlReader.attributes().value("fileExtensions").toString().trimmed());
    moduleParamPrivate->Type = xmlReader.attributes().value("type").toString().trimmed();
  }
};


#endif // CTKCMDLINEMODULEPARAMETERPARSERS_P_H
