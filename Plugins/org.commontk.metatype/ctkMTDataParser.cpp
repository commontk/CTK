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


#include "ctkMTDataParser_p.h"

#include "ctkAttributeDefinitionImpl_p.h"
#include "ctkMTMsg_p.h"

#include <service/log/ctkLogService.h>

#include <QCoreApplication>
#include <QDebug>

const QString ctkMTDataParser::METADATA = "MetaData";
const QString ctkMTDataParser::LOCALIZATION = "localization";
const QString ctkMTDataParser::CONTEXT = "context";
const QString ctkMTDataParser::OCD = "OCD";
const QString ctkMTDataParser::ICON = "Icon";
const QString ctkMTDataParser::AD = "AD";
const QString ctkMTDataParser::CARDINALITY = "cardinality";
const QString ctkMTDataParser::OPTION = "Option";
const QString ctkMTDataParser::LABEL = "label";
const QString ctkMTDataParser::VALUE = "value";
const QString ctkMTDataParser::MIN = "min";
const QString ctkMTDataParser::MAX = "max";
const QString ctkMTDataParser::TYPE = "type";
const QString ctkMTDataParser::SIZE = "size";
const QString ctkMTDataParser::ID = "id";
const QString ctkMTDataParser::NAME = "name";
const QString ctkMTDataParser::DESCRIPTION = "description";
const QString ctkMTDataParser::RESOURCE = "resource";
const QString ctkMTDataParser::PID = "pid";
const QString ctkMTDataParser::DEFAULT = "default";
const QString ctkMTDataParser::ADREF = "adref";
const QString ctkMTDataParser::CONTENT = "content";
const QString ctkMTDataParser::FACTORY = "factoryPid";
const QString ctkMTDataParser::PLUGIN = "plugin";
const QString ctkMTDataParser::OPTIONAL = "optional";
const QString ctkMTDataParser::OBJECT = "Object";
const QString ctkMTDataParser::OCDREF = "ocdref";
const QString ctkMTDataParser::ATTRIBUTE = "Attribute";
const QString ctkMTDataParser::DESIGNATE = "Designate";
const QString ctkMTDataParser::MERGE = "merge";
const QString ctkMTDataParser::REQUIRED = "required";

const QString ctkMTDataParser::INTEGER = "Integer";
const QString ctkMTDataParser::STRING = "String";
const QString ctkMTDataParser::DOUBLE = "Double";
const QString ctkMTDataParser::LONG = "Long";
const QString ctkMTDataParser::CHAR = "Char";
const QString ctkMTDataParser::BOOLEAN = "Boolean";
const QString ctkMTDataParser::PASSWORD = "Password";


ctkMTDataParser::ctkMTDataParser(const QSharedPointer<ctkPlugin>& plugin, QIODevice* device, ctkLogService* logger)
  : _dp_plugin(plugin), _dp_xmlReader(device), logger(logger)
{

}

QHash<QString, ctkObjectClassDefinitionImplPtr> ctkMTDataParser::doParse()
{
  CTK_DEBUG(logger) << "Starting to parse metadata";
  while (_dp_xmlReader.readNextStartElement())
  {
    QStringRef name = _dp_xmlReader.name();
    if (name.compare(METADATA, Qt::CaseInsensitive) == 0)
    {
      metaDataHandler();
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }

  if (_dp_xmlReader.hasError() &&
      !(_dp_xmlReader.atEnd() && _dp_xmlReader.error() == QXmlStreamReader::PrematureEndOfDocumentError))
  {
    errorHandler();
    return QHash<QString, ctkObjectClassDefinitionImplPtr>();
  }

  return _dp_pid_to_OCDs;
}

void ctkMTDataParser::metaDataHandler()
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(METADATA, Qt::CaseInsensitive) == 0);

  QXmlStreamAttributes attributes = _dp_xmlReader.attributes();
  _dp_localization = attributes.value(LOCALIZATION).toString();
  _dp_context = attributes.value(CONTEXT).toString();
  // The variables "_dp_localization" and "_dp_context" will be used within
  // ocdHandler() and attributeDefinitionHandler() later.

  _dp_xmlReader.readNext();
  while(!_dp_xmlReader.hasError() && _dp_xmlReader.readNextStartElement())
  {
    QStringRef name = _dp_xmlReader.name();
    if (name.compare(DESIGNATE, Qt::CaseInsensitive) == 0)
    {
      designateHandler();
    }
    else if (name.compare(OCD, Qt::CaseInsensitive) == 0)
    {
      ocdHandler();
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }

  if (_dp_xmlReader.hasError()) return;

  if (_dp_designateInfos.isEmpty())
  {
    // Schema defines at least one DESIGNATE is required.
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ELEMENT).arg(DESIGNATE);
    _dp_xmlReader.raiseError(msg);
    return;
  }

  for (int i = 0; i < _dp_designateInfos.size(); ++i)
  {
    const DesignateInfo& designateInfo = _dp_designateInfos[i];
    ctkObjectClassDefinitionImplPtr ocd = _dp_OCDs.value(designateInfo._ocdref);
    if (ocd)
    {
      if (designateInfo._factory_val.isEmpty())
      {
        ocd->setType(ctkObjectClassDefinitionImpl::PID);
        _dp_pid_to_OCDs.insert(designateInfo._pid_val, ocd);
      }
      else
      {
        ocd->setType(ctkObjectClassDefinitionImpl::FPID);
        _dp_pid_to_OCDs.insert(designateInfo._factory_val, ocd);
      }
    }
    else
    {
       QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::OCD_ID_NOT_FOUND).arg(designateInfo._ocdref);
       _dp_xmlReader.raiseError(msg);
       return;
    }
  }
}

void ctkMTDataParser::designateHandler()
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(DESIGNATE, Qt::CaseInsensitive) == 0);

  DesignateInfo info;
  QXmlStreamAttributes atts = _dp_xmlReader.attributes();

  info._pid_val = atts.value(PID).toString();
  if (info._pid_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(PID).arg(_dp_xmlReader.name().toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }
  info._factory_val = atts.value(FACTORY).toString();

  info._plugin_val = atts.value(PLUGIN).toString();
  if (info._plugin_val.isNull())
  {
    // Not a problem because PLUGIN is an optional attribute.
  }

  QString optional_str = atts.value(OPTIONAL).toString();
  if (optional_str.isNull())
  {
    // Not a problem, because OPTIONAL is an optional attribute.
    // The default value is "false".
    info._optional_val = false;
  }
  else
  {
    info._optional_val = toBool(optional_str);
  }

  QString merge_str = atts.value(MERGE).toString();
  if (merge_str.isNull())
  {
    // Not a problem, because MERGE is an optional attribute.
    // The default value is "false".
    info._merge_val = false;
  }
  else
  {
    info._merge_val = toBool(merge_str);
  }

  while (!_dp_xmlReader.hasError() && _dp_xmlReader.readNextStartElement())
  {
    QStringRef name = _dp_xmlReader.name();
    if (name.compare(OBJECT, Qt::CaseInsensitive) == 0)
    {
     objectHandler(info);
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }

  if (_dp_xmlReader.hasError()) return;

  if (info._ocdref.isEmpty())
  {
    // Schema defines at least one OBJECT is required.
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ELEMENT).arg(OBJECT).arg(info._pid_val);
    _dp_xmlReader.raiseError(msg);
    return;
  }

  _dp_designateInfos.push_back(info);
}

void ctkMTDataParser::ocdHandler()
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(OCD, Qt::CaseInsensitive) == 0);

  QStringRef name = _dp_xmlReader.name();
  QXmlStreamAttributes atts = _dp_xmlReader.attributes();
  QString ocd_name_val = atts.value(NAME).toString();
  if (ocd_name_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(NAME).arg(name.toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }

  QString ocd_description_val = atts.value(DESCRIPTION).toString();
  if (ocd_description_val.isNull())
  {
    // Not a problem, because DESCRIPTION is an optional attribute.
  }

  QString refID = atts.value(ID).toString();
  if (refID.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(ID).arg(name.toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }

  ctkObjectClassDefinitionImplPtr ocd(new ctkObjectClassDefinitionImpl(
                                        ocd_name_val, ocd_description_val,
                                        refID, _dp_localization, _dp_context));

  QList<ctkAttributeDefinitionImplPtr> ad_list;
  while (!_dp_xmlReader.hasError() && _dp_xmlReader.readNextStartElement())
  {
    QStringRef name2 = _dp_xmlReader.name();
    if (name2.compare(AD, Qt::CaseInsensitive) == 0)
    {
      attributeDefinitionHandler(ad_list);
    }
    else if (name2.compare(ICON, Qt::CaseInsensitive) == 0)
    {
      ctkMTIcon icon = iconHandler();
      if (!_dp_xmlReader.hasError())
      {
        // Because XML schema allows at most one icon for
        // one OCD, if more than one icons are read from
        // MetaData, then only the final icon will be kept.
        ocd->setIcon(icon);
      }
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name2.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }

  if (_dp_xmlReader.hasError()) return;

  if (ad_list.isEmpty())
  {
    // Schema defines at least one AD is required.
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ELEMENT).arg(AD).arg(refID);
    _dp_xmlReader.raiseError(msg);
    return;
  }
  // OCD gets all parsed ADs.
  foreach (ctkAttributeDefinitionImplPtr ad, ad_list)
  {
    ocd->addAttributeDefinition(ad, ad->isRequired());
  }

  _dp_OCDs.insert(refID, ocd);
}

void ctkMTDataParser::objectHandler(DesignateInfo& designateInfo)
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(OBJECT, Qt::CaseInsensitive) == 0);

  designateInfo._ocdref = _dp_xmlReader.attributes().value(OCDREF).toString();
  if (designateInfo._ocdref.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(OCDREF).arg(_dp_xmlReader.name().toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }

  while (!_dp_xmlReader.hasError() && _dp_xmlReader.readNextStartElement())
  {
    QStringRef name = _dp_xmlReader.name();
    if (name.compare(ATTRIBUTE, Qt::CaseInsensitive) == 0)
    {
      attributeHandler();
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }
}

void ctkMTDataParser::attributeHandler()
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(ATTRIBUTE, Qt::CaseInsensitive) == 0);
  while (_dp_xmlReader.readNextStartElement()) _dp_xmlReader.skipCurrentElement();
}

void ctkMTDataParser::attributeDefinitionHandler(QList<ctkAttributeDefinitionImplPtr>& ad_list)
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(AD, Qt::CaseInsensitive) == 0);

  QString name = _dp_xmlReader.name().toString();
  QXmlStreamAttributes atts = _dp_xmlReader.attributes();
  QString ad_name_val = atts.value(NAME).toString();
  if (ad_name_val.isNull())
  {
    // Not a problem, because NAME is an optional attribute.
  }

  QString ad_description_val = atts.value(DESCRIPTION).toString();
  if (ad_description_val.isNull())
  {
    // Not a problem, because DESCRIPTION is an optional attribute.
  }

  QString ad_id_val = atts.value(ID).toString();
  if (ad_id_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(ID).arg(name);
    _dp_xmlReader.raiseError(msg);
    return;
  }

  int dataType;
  QStringRef ad_type_val = atts.value("", TYPE);
  if (ad_type_val.compare(STRING, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::String;
  }
  else if (ad_type_val.compare(LONG, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::LongLong;
  }
  else if (ad_type_val.compare(DOUBLE, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::Double;
  }
  else if (ad_type_val.compare(INTEGER, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::Int;
  }
  else if (ad_type_val.compare(CHAR, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::Char;
  }
  else if (ad_type_val.compare(BOOLEAN, Qt::CaseInsensitive) == 0)
  {
    dataType = QVariant::Bool;
  }
  else if (ad_type_val.compare(PASSWORD, Qt::CaseInsensitive) == 0)
  {
    dataType = ctkAttributeDefinition::PASSWORD;
  }
  else
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(TYPE).arg(name);
    _dp_xmlReader.raiseError(msg);
    return;
  }

  QString ad_cardinality_str = atts.value(CARDINALITY).toString();
  int ad_cardinality_val = 0;
  if (ad_cardinality_str.isNull())
  {
    // Not a problem, because CARDINALITY is an optional attribute.
    // And the default value is 0.
  }
  else
  {
    ad_cardinality_val = ad_cardinality_str.toInt();
  }

  QString ad_min_val = atts.value(MIN).toString();
  if (ad_min_val.isNull())
  {
    // Not a problem, because MIN is an optional attribute.
  }

  QString ad_max_val = atts.value(MAX).toString();
  if (ad_max_val.isNull())
  {
    // Not a problem, because MAX is an optional attribute.
  }

  QString ad_defaults_str = atts.value(DEFAULT).toString();
  if (ad_defaults_str.isNull())
  {
    // Not a problem, because DEFAULT is an optional attribute.
    if (ad_cardinality_val == 0)
    {
      // But when it is not assigned, CARDINALITY cannot be '0'.
      QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::NULL_DEFAULTS);
      _dp_xmlReader.raiseError(msg);
      return;
    }
  }

  QString ad_required_val = atts.value(REQUIRED).toString();
  if (ad_required_val.isNull())
  {
    // Not a problem, because REQUIRED is an optional attribute.
    // And the default value is 'true'.
    ad_required_val = "true";
  }

  ctkAttributeDefinitionImplPtr ad(new ctkAttributeDefinitionImpl(
                                     ad_id_val, ad_name_val, ad_description_val, dataType,
                                     ad_cardinality_val, convert(ad_min_val, dataType), convert(ad_max_val, dataType),
                                     toBool(ad_required_val), _dp_localization, _dp_context, logger));

  if (ad_cardinality_val == 0)
  {
    // Attribute DEFAULT has one and only one occurance.
    ad->setDefaultValue(QStringList(ad_defaults_str), false);
  }
  else
  {
    // Attribute DEFAULT is a comma delimited list.
    ad->setDefaultValue(ad_defaults_str, false);
  }

  QStringList optionLabels;
  QStringList optionValues;
  while (!_dp_xmlReader.hasError() && _dp_xmlReader.readNextStartElement())
  {
    QStringRef name2 = _dp_xmlReader.name();
    if (name2.compare(OPTION, Qt::CaseInsensitive) == 0)
    {
      OptionInfo optionInfo;
      optionHandler(optionInfo);
      if (!_dp_xmlReader.hasError())
      {
        optionLabels.push_back(optionInfo._label_val);
        optionValues.push_back(optionInfo._value_val);
      }
    }
    else
    {
      CTK_WARN(logger) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::UNEXPECTED_ELEMENT).arg(name2.toString());
      _dp_xmlReader.skipCurrentElement();
    }
  }

  if (_dp_xmlReader.hasError()) return;

  ad->setOption(optionLabels, optionValues, true);
  ad_list.push_back(ad);
}

ctkMTIcon ctkMTDataParser::iconHandler()
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(ICON, Qt::CaseInsensitive) == 0);

  QXmlStreamAttributes atts = _dp_xmlReader.attributes();
  QString icon_resource_val = atts.value(RESOURCE).toString();
  if (icon_resource_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(RESOURCE).arg(_dp_xmlReader.name().toString());
    _dp_xmlReader.raiseError(msg);
    return ctkMTIcon();
  }

  QString icon_size_val = atts.value(SIZE).toString();
  if (icon_size_val.isNull())
  {
    // Not a problem, because SIZE is an optional attribute.
    icon_size_val = "0";
  }

  while (_dp_xmlReader.readNextStartElement()) _dp_xmlReader.skipCurrentElement();
  return ctkMTIcon(icon_resource_val, icon_size_val.toInt(), _dp_plugin);
}

void ctkMTDataParser::optionHandler(OptionInfo& optionInfo)
{
  Q_ASSERT(_dp_xmlReader.isStartElement() && _dp_xmlReader.name().compare(OPTION, Qt::CaseInsensitive) == 0);

  QXmlStreamAttributes atts = _dp_xmlReader.attributes();
  optionInfo._label_val = atts.value(LABEL).toString();
  if (optionInfo._label_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(LABEL).arg(_dp_xmlReader.name().toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }

  optionInfo._value_val = atts.value(VALUE).toString();
  if (optionInfo._value_val.isNull())
  {
    QString msg = QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::MISSING_ATTRIBUTE).arg(VALUE).arg(_dp_xmlReader.name().toString());
    _dp_xmlReader.raiseError(msg);
    return;
  }

  while (_dp_xmlReader.readNextStartElement()) _dp_xmlReader.skipCurrentElement();
}

void ctkMTDataParser::errorHandler()
{
  switch (_dp_xmlReader.error())
  {
  case QXmlStreamReader::NotWellFormedError:
    CTK_WARN(logger) << "XML Document not well formed at line " << _dp_xmlReader.lineNumber() << ", col " << _dp_xmlReader.columnNumber();
    break;
  case QXmlStreamReader::PrematureEndOfDocumentError:
    CTK_WARN(logger) << "Premature end of XML Document at line " << _dp_xmlReader.lineNumber() << ", col " << _dp_xmlReader.columnNumber();
    break;
  case QXmlStreamReader::UnexpectedElementError:
    CTK_WARN(logger) << "Unexpected element at line " << _dp_xmlReader.lineNumber() << ", col " << _dp_xmlReader.columnNumber();
    break;
  case QXmlStreamReader::CustomError:
    CTK_ERROR(logger) << _dp_xmlReader.errorString() << "at line" << _dp_xmlReader.lineNumber() << ", col " << _dp_xmlReader.columnNumber();
    break;
  default:
    break;
  }
}

bool ctkMTDataParser::toBool(const QString& str)
{
  if (str.isEmpty()) return false;
  if (str.trimmed().compare("true", Qt::CaseInsensitive)) return true;
  if (str.trimmed().compare("1")) return true;
  return false;
}

QVariant ctkMTDataParser::convert(const QString& value, int type)
{
  if (value.isNull())
  {
    return QVariant();
  }

  if (type == ctkAttributeDefinition::PASSWORD || type == QVariant::String)
  {
    // PASSWORD should be treated like STRING.
    // Both the min and max of STRING are Integers.
    return QVariant::fromValue<int>(value.toInt());
  }
  else if (type == QVariant::LongLong)
  {
    return QVariant::fromValue<qlonglong>(value.toLongLong());
  }
  else if (type == QVariant::Int)
  {
    return QVariant::fromValue<int>(value.toInt());
  }
  else if (type == QVariant::Char)
  {
    return QVariant::fromValue<QChar>(value.at(0));
  }
  else if (type == QVariant::Double)
  {
    return QVariant::fromValue<double>(value.toDouble());
  }
  else if (type == QVariant::Bool)
  {
    return QVariant::fromValue<bool>(toBool(value));
  }
  else
  {
    // Unknown data type
    return QVariant();
  }
}

