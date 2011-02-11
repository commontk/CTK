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


#ifndef CTKATTRIBUTEDEFINITION_H
#define CTKATTRIBUTEDEFINITION_H

#include <QSharedPointer>
#include <QMetaType>
#include <QString>
#include <QVariant>

#include <ctkPluginFrameworkExport.h>

/**
 * \ingroup Metatype
 *
 * An interface to describe an attribute.
 *
 * <p>
 * An <code>ctkAttributeDefinition</code> object defines a description of the data type
 * of a property/attribute.
 */
struct CTK_PLUGINFW_EXPORT ctkAttributeDefinition
{
  typedef QVariant::Type Type;

  static const int PASSWORD;

  /**
   * The <code>Password</code> type.
   *
   * Attributes of this type must be stored as <code>QString</code>
   * or <code>QStringList</code> objects depending on
   * getCardinality(). A <code>Password</code> must be treated as a string but the type
   * can be used to disguise the information when displayed to a user to
   * prevent others from seeing it.
   */
  struct Password : public QString {
    Password() : QString() {}
    Password(const QString& pwd) : QString(pwd) {}
    Password(const char* pwd) : QString(pwd) {}
  };

  virtual ~ctkAttributeDefinition() {}

  /**
   * Get the name of the attribute. This name may be localized.
   *
   * @return The localized name of the definition.
   */
  virtual QString getName() const = 0;

  /**
   * Unique identity for this attribute.
   *
   * Attributes share a global namespace in the registry. E.g. an attribute
   * <code>cn</code> or <code>commonName</code> must always be a <code>QString</code> and the
   * semantics are always a name of some object. They share this aspect with
   * LDAP/X.500 attributes. In these standards the OSI Object Identifier (OID)
   * is used to uniquely identify an attribute. If such an OID exists, (which
   * can be requested at several standard organisations and many companies
   * already have a node in the tree) it can be returned here. Otherwise, a
   * unique id should be returned which can be a class name combined witha a reverse
   * domain name or generated with a GUID algorithm. Note that all LDAP
   * defined attributes already have an OID. It is strongly advised to define
   * the attributes from existing LDAP schemes which will give the OID. Many
   * such schemes exist ranging from postal addresses to DHCP parameters.
   *
   * @return The id or oid
   */
  virtual QString getID() const = 0;

  /**
   * Return a description of this attribute.
   *
   * The description may be localized and must describe the semantics of this
   * type and any constraints.
   *
   * @return The localized description of the definition.
   */
  virtual QString getDescription() const = 0;

  /**
   * Return the cardinality of this attribute.
   *
   * The CTK Plugin environment handles multi valued attributes in QList
   * objects. The return value is defined as follows:
   *
   * <pre>
   *
   *    x = -1     no limit, store in QList
   *    x &gt; 0   x = max occurrences, store in QList
   *    x = 0      1 occurrence required
   *
   * </pre>
   *
   * @return The cardinality of this attribute.
   */
  virtual int getCardinality() const = 0;

  /**
   * Return the type for this attribute.
   *
   * <p>
   * The following types from QVariant::Type are supported:
   * QVariant::String, QVariant::LongLong, QVariant::Int, QVariant::Char,
   * QVariant::Double, QVariant::Bool, QVariant::UserType.
   *
   * <p>
   * QVariant::UserType maps to ctkAttributeDefinition::Password only.
   *
   * @return The type for this attribute.
   */
  virtual int getType() const = 0;

  /**
   * Return a list of option values that this attribute can take.
   *
   * <p>
   * If the function returns an empty QList, there are no option values
   * available.
   *
   * <p>
   * Each value must be acceptable to validate() (return "") and must be a
   * <code>QString</code> object that can be converted to the data type defined by
   * getType() for this attribute.
   *
   * <p>
   * This list must be in the same sequence as <code>getOptionLabels()</code>. I.e.
   * for each index i in <code>getOptionValues</code>, i in <code>getOptionLabels()</code>
   * should be the label.
   *
   * @return A list values
   */
  virtual QStringList getOptionValues() const = 0;

  /**
   * Return a list of labels of option values.
   *
   * <p>
   * The purpose of this method is to allow menus with localized labels. It is
   * associated with <code>getOptionValues</code>. The labels returned here are
   * ordered in the same way as the values in that method.
   *
   * <p>
   * If the function returns an empty list, there are no option labels
   * available.
   * <p>
   * This list must be in the same sequence as the <code>getOptionValues()</code>
   * method. I.e. for each index i in <code>getOptionLabels</code>, i in <code>
   * getOptionValues()</code> should be the associated value.
   *
   * @return A list values
   */
  virtual QStringList getOptionLabels() const = 0;

  /**
   * Validate an attribute in <code>QString</code> form.
   *
   * An attribute might be further constrained in value. This method will
   * attempt to validate the attribute according to these constraints. It can
   * return three different values:
   *
   * <pre>
   *  a null QString     No validation present
   *  an empty QString   No problems detected
   *  "..."              A localized description of why the value is wrong
   * </pre>
   *
   * @param value The value before turning it into the basic data type
   * @return a null QString, an empty QString, or another QString
   */
  virtual QString validate(const QString& value) const = 0;

  /**
   * Return a default for this attribute.
   *
   * The object must be of the appropriate type as defined by the cardinality
   * and <code>getType()</code>. The return type is a QStringList
   * object whose entries can be converted to the appropriate type. The cardinality of
   * the return list must follow the absolute cardinality of this type. E.g.
   * if the cardinality = 0, the list must contain 1 element. If the
   * cardinality is 1, it must contain 0 or 1 elements. Note that the special case of a 0
   * cardinality, meaning a single value, does not allow lists of
   * 0 elements, except for signalling that no default exists.
   *
   * @return Return a default value or an empty list if no default exists.
   */
  virtual QStringList getDefaultValue() const = 0;
};


/**
 * \ingroup Metatype
 */
typedef QSharedPointer<ctkAttributeDefinition> ctkAttributeDefinitionPtr;

Q_DECLARE_METATYPE(ctkAttributeDefinition::Password)


#endif // CTKATTRIBUTEDEFINITION_H
