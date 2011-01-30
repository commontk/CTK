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


#ifndef CTKMTDATAPARSER_P_H
#define CTKMTDATAPARSER_P_H

#include "ctkObjectClassDefinitionImpl_p.h"
#include "ctkAttributeDefinitionImpl_p.h"

#include <QXmlStreamReader>

class QIODevice;
struct ctkLogService;

/**
 * Meta XML Data Parser
 */
class ctkMTDataParser
{
private:

  static const QString METADATA; // = "MetaData"
  static const QString LOCALIZATION; // = "localization"
  static const QString CONTEXT; // = "context"
  static const QString OCD; // = "OCD"
  static const QString ICON; // = "Icon"
  static const QString AD; // = "AD"
  static const QString CARDINALITY; // = "cardinality"
  static const QString OPTION; // = "Option"
  static const QString LABEL; // = "label"
  static const QString VALUE; // = "value"
  static const QString MIN; // = "min"
  static const QString MAX; // = "max"
  static const QString TYPE; // = "type"
  static const QString SIZE; // = "size"
  static const QString ID; // = "id"
  static const QString NAME; // = "name"
  static const QString DESCRIPTION; // = "description"
  static const QString RESOURCE; // = "resource"
  static const QString PID; // = "pid"
  static const QString DEFAULT; // = "default"
  static const QString ADREF; // = "adref"
  static const QString CONTENT; // = "content"
  static const QString FACTORY; // = "factoryPid"
  static const QString PLUGIN; // = "plugin"
  static const QString OPTIONAL; // = "optional"
  static const QString OBJECT; // = "Object"
  static const QString OCDREF; // = "ocdref"
  static const QString ATTRIBUTE; // = "Attribute"
  static const QString DESIGNATE; // = "Designate"
  static const QString MERGE; // = "merge"
  static const QString REQUIRED; // = "required"

  static const QString INTEGER; // = "Integer"
  static const QString STRING; // = "String"
  static const QString DOUBLE; // = "Double"
  static const QString LONG; // = "Long"
  static const QString CHAR; // = "Char"
  static const QString BOOLEAN; // = "Boolean"
  static const QString PASSWORD; // = "Password"

  struct DesignateInfo
  {
    QString _pid_val;
    QString _factory_val;
    QString _plugin_val;
    bool _optional_val;
    bool _merge_val;
    QString _ocdref;

    DesignateInfo() : _optional_val(false), _merge_val(false)
    {}
  };

  QList<DesignateInfo> _dp_designateInfos;

  struct OptionInfo
  {
    QString _label_val;
    QString _value_val;
  };

  QSharedPointer<ctkPlugin> _dp_plugin;
  QXmlStreamReader _dp_xmlReader;

  // ObjectClassDefinitions in DataParser class w/ corresponding reference keys
  QHash<QString, ctkObjectClassDefinitionImplPtr> _dp_OCDs;
  // pid to ObjectClassDefinitions in DataParser class as a Hashtable
  QHash<QString, ctkObjectClassDefinitionImplPtr> _dp_pid_to_OCDs;
  // Localization in ctkMTDataParser class
  QString _dp_localization;
  // Localization context
  QString _dp_context;

  // Default visibility to avoid a plethora of synthetic accessor method warnings.
  ctkLogService* const logger;

public:

  /*
   * Constructor of class ctkMTDataParser.
   */
  ctkMTDataParser(const QSharedPointer<ctkPlugin>& plugin, QIODevice* device, ctkLogService* logger);

  /*
   * Main method to parse specific MetaData file.
   */
  QHash<QString, ctkObjectClassDefinitionImplPtr> doParse();

private:

  void metaDataHandler();
  void designateHandler();
  void ocdHandler();
  void objectHandler(DesignateInfo& designateInfo);
  void attributeHandler();
  void attributeDefinitionHandler(QList<ctkAttributeDefinitionImplPtr>& ad_list);
  ctkMTIcon iconHandler();
  void optionHandler(OptionInfo& optionInfo);

  void errorHandler();

  static bool toBool(const QString& str);

  /*
   * Convert QString for expected data type.
   */
  static QVariant convert(const QString& value, int type);

};

#endif // CTKMTDATAPARSER_P_H
