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


#ifndef CTKATTRIBUTEDEFINITIONIMPL_P_H
#define CTKATTRIBUTEDEFINITIONIMPL_P_H

#include <service/metatype/ctkAttributeDefinition.h>

#include "ctkMTLocalizationElement_p.h"

#include <QStringList>
#include <QVariant>

struct ctkLogService;

/**
 * Implementation of ctkAttributeDefintion
 */
class ctkAttributeDefinitionImpl : public ctkAttributeDefinition
{

private:

  static const QChar SEPARATE;
  static const QChar CONTROL;

  QString _name;
  QString _id;
  QString _description;
  int _cardinality;
  int _dataType;
  QVariant _minValue;
  QVariant _maxValue;
  bool _isRequired;

  ctkMTLocalizationElement _locElem;

  QStringList _defaults;
  QStringList _values;
  QStringList _labels;

  ctkLogService* const _logger;

public:

  /**
   * Constructor of class ctkAttributeDefinitionImpl.
   */
  ctkAttributeDefinitionImpl(const QString& id, const QString& name, const QString& description,
                             int type, int cardinality, const QVariant& min, const QVariant& max,
                             bool isRequired, const QString& localization, const QString& context,
                             ctkLogService* logger);

  /*
   * @see ctkAttributeDefinition#getName()
   */
  QString getName() const;

  /**
   * Method to set the name of AttributeDefinition.
   */
  void setName(const QString& name);

  /*
   * @see ctkAttributeDefinition#getID()
   */
  QString getID() const;

  /**
   * Method to set the ID of ctkAttributeDefinition.
  */
  void setID(const QString& id);

  /*
   * @see ctkAttributeDefinition#getDescription()
  */
  QString getDescription() const;

  /**
   * Method to set the description of ctkAttributeDefinition.
   */
  void setDescription(const QString& description);

  /*
   * @see ctkAttributeDefinition#getCardinality()
   */
  int getCardinality() const;

  /**
   * Method to set the cardinality of ctkAttributeDefinition.
   */
  void setCardinality(int cardinality);

  /*
   * @see ctkAttributeDefinition#getType()
   */
  int getType() const;

  /**
   * Method to set the data type of ctkAttributeDefinition.
   */
  void setType(int type);

  /**
   * Method to get the required flag of ctkAttributeDefinition.
   */
  bool isRequired() const;

  /**
   * Method to set the required flag of ctkAttributeDefinition.
   */
  void setRequired(bool isRequired);

  /*
   * @see ctkAttributeDefinition#getOptionLabels()
   */
  QStringList getOptionLabels() const;

  /*
   * @see ctkAttributeDefinition#getOptionValues()
   */
  QStringList getOptionValues() const;

  /**
   * Method to set the Option values of ctkAttributeDefinition.
   */
  void setOption(const QStringList& labels, const QStringList& values,
                 bool needValidation);

  /*
   * @see ctkAttributeDefinition#getDefaultValue()
   */
  QStringList getDefaultValue() const;

  /**
   * Method to set the default value of ctkAttributeDefinition.
   * The given parameter is a comma delimited list needed to be parsed.
   */
  void setDefaultValue(const QString& defaults_str, bool needValidation);

  /**
   * Method to set the default value of ctkAttributeDefinition.
   * The given parameter is a String array of multi values.
   */
  void setDefaultValue(const QStringList& defaults, bool needValidation);

  /**
   * Method to set the validation value - min of ctkAttributeDefinition.
   */
  void setMinValue(const QVariant& minValue);

  /**
   * Method to set the validation value - max of ctkAttributeDefinition.
   */
  void setMaxValue(const QVariant& maxValue);

  /**
   * Method to set the plugin localization object.
   */
  void setPluginLocalization(const ctkPluginLocalization& pluginLoc);

  /*
   * @see ctkAttributeDefinition#validate(const QString&)
   */
  QString validate(const QString& value) const;

private:

  /**
   * Internal Method - to validate data in range.
   */
  QString validateRange(const QString& value) const;

  QStringList tokenizeValues(const QString& values) const;
};

typedef QSharedPointer<ctkAttributeDefinitionImpl> ctkAttributeDefinitionImplPtr;

#endif // CTKATTRIBUTEDEFINITIONIMPL_P_H
