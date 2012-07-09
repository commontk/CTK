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

namespace {

static bool parseBooleanAttribute(const QStringRef& attrValue)
{
  if (attrValue.compare("true", Qt::CaseInsensitive) == 0 ||
      attrValue.compare("1") == 0)
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

  virtual ctkCmdLineModuleParameter* parse(QXmlStreamReader &xmlReader)
  {
    ctkCmdLineModuleParameter* moduleParam = this->createModuleParameter();

    this->handleAttributes(moduleParam, xmlReader);

    while(xmlReader.readNextStartElement())
    {
      this->handleSubElement(moduleParam, xmlReader);
    }

    return moduleParam;
  }

protected:

  virtual ctkCmdLineModuleParameter* createModuleParameter()
  {
    return new ctkCmdLineModuleParameter;
  }

  virtual void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    // handle common attributes
    moduleParam->setHidden(parseBooleanAttribute(xmlReader.attributes().value("hidden")));
  }

  virtual bool handleSubElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    // handle common sub-elements

    QStringRef name = xmlReader.name();

    if (name.compare("name", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setName(xmlReader.readElementText().trimmed());
    }
    else if (name.compare("description", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setDescription(xmlReader.readElementText().trimmed());
    }
    else if (name.compare("label", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setLabel(xmlReader.readElementText().trimmed());
    }
    else if (name.compare("default", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setDefaultValue(xmlReader.readElementText().trimmed());
    }
    else if (name.compare("flag", Qt::CaseInsensitive) == 0)
    {
      QString flag = xmlReader.readElementText().trimmed();
      if (flag.startsWith('-')) flag = flag.remove(0, 1);
      moduleParam->setFlag(flag);
      moduleParam->setFlagAliasesAsString(xmlReader.attributes().value("alias").toString());
      moduleParam->setDeprecatedFlagAliasesAsString(xmlReader.attributes().value("deprecatedalias").toString());
    }
    else if (name.compare("longflag", Qt::CaseInsensitive) == 0)
    {
      QString longFlag = xmlReader.readElementText().trimmed();
      if (longFlag.startsWith('-')) longFlag = longFlag.remove(0, 1);
      moduleParam->setLongFlag(longFlag);
      moduleParam->setLongFlagAliasesAsString(xmlReader.attributes().value("alias").toString());
      moduleParam->setDeprecatedLongFlagAliasesAsString(xmlReader.attributes().value("deprecatedalias").toString());
    }
    else if (name.compare("index", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setIndex(xmlReader.readElementText().toInt());
    }
    else
    {
      xmlReader.skipCurrentElement();
      return false;
    }

    return true;
  }

  bool handleConstraintsElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    moduleParam->setConstraints(true);
    while(xmlReader.readNextStartElement())
    {
      QStringRef constraintElem = xmlReader.name();
      if (constraintElem.compare("minimum", Qt::CaseInsensitive) == 0)
      {
        moduleParam->setMinimum(xmlReader.readElementText().trimmed());
      }
      else if (constraintElem.compare("maximum", Qt::CaseInsensitive) == 0)
      {
        moduleParam->setMaximum(xmlReader.readElementText().trimmed());
      }
      else if (constraintElem.compare("step", Qt::CaseInsensitive) == 0)
      {
        moduleParam->setStep(xmlReader.readElementText().trimmed());
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

  void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleParameterParser::handleAttributes(moduleParam, xmlReader);
    moduleParam->setHidden(parseBooleanAttribute(xmlReader.attributes().value("multiple")));
  }
};

class ctkCmdLineModuleScalarVectorParameterParser : public ctkCmdLineModuleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (name.compare("constraints", Qt::CaseInsensitive) == 0)
    {
      return handleConstraintsElement(moduleParam, xmlReader);
    }
    else
    {
      return ctkCmdLineModuleParameterParser::handleSubElement(moduleParam, xmlReader);
    }
  }
};

class ctkCmdLineModuleScalarParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (name.compare("constraints", Qt::CaseInsensitive) == 0)
    {
      return handleConstraintsElement(moduleParam, xmlReader);
    }
    else
    {
      return ctkCmdLineModuleMultipleParameterParser::handleSubElement(moduleParam, xmlReader);
    }
  }
};

class ctkCmdLineModuleEnumerationParameterParser : public ctkCmdLineModuleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (name.compare("element", Qt::CaseInsensitive) == 0)
    {
      moduleParam->addElement(xmlReader.readElementText().trimmed());
      return true;
    }
    else
    {
      return ctkCmdLineModuleParameterParser::handleSubElement(moduleParam, xmlReader);
    }
  }
};

class ctkCmdLineModulePointParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParam, xmlReader);
    moduleParam->setCoordinateSystem(xmlReader.attributes().value("coordinateSystem").toString().trimmed());
  }
};

class ctkCmdLineModuleChannelParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  bool handleSubElement(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    QStringRef name = xmlReader.name();

    if (name.compare("channel", Qt::CaseInsensitive) == 0)
    {
      moduleParam->setChannel(xmlReader.readElementText().trimmed());
      return true;
    }
    else
    {
      return ctkCmdLineModuleMultipleParameterParser::handleSubElement(moduleParam, xmlReader);
    }
  }
};

class ctkCmdLineModuleFileParameterParser : public ctkCmdLineModuleChannelParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleChannelParameterParser::handleAttributes(moduleParam, xmlReader);
    moduleParam->setFileExtensionsAsString(xmlReader.attributes().value("fileExtensions").toString().trimmed());
  }
};

class ctkCmdLineModuleGeometryParameterParser : public ctkCmdLineModuleMultipleParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleMultipleParameterParser::handleAttributes(moduleParam, xmlReader);
    moduleParam->setFileExtensionsAsString(xmlReader.attributes().value("fileExtensions").toString().trimmed());
    moduleParam->setType(xmlReader.attributes().value("type").toString().trimmed());
  }
};

class ctkCmdLineModuleImageParameterParser : public ctkCmdLineModuleChannelParameterParser
{

protected:

  void handleAttributes(ctkCmdLineModuleParameter* moduleParam, QXmlStreamReader& xmlReader)
  {
    ctkCmdLineModuleChannelParameterParser::handleAttributes(moduleParam, xmlReader);
    moduleParam->setFileExtensionsAsString(xmlReader.attributes().value("fileExtensions").toString().trimmed());
    moduleParam->setType(xmlReader.attributes().value("type").toString().trimmed());
  }
};


#endif // CTKCMDLINEMODULEPARAMETERPARSERS_P_H
