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


#ifndef CTKOBJECTCLASSDEFINITION_H
#define CTKOBJECTCLASSDEFINITION_H

#include "ctkAttributeDefinition.h"

/**
 * \ingroup Metatype
 *
 * Description for the data type information of an objectclass.
 */
struct ctkObjectClassDefinition
{

  enum Filter {
    /**
     * Argument for <code>getAttributeDefinitions()</code>.
     * <p>
     * <code>REQUIRED</code> indicates that only the required definitions are
     * returned.
     */
    REQUIRED,
    /**
     * Argument for <code>getAttributeDefinitions()</code>.
     * <p>
     * <code>OPTIONAL</code> indicates that only the optional definitions are
     * returned.
     */
    OPTIONAL,
    /**
     * Argument for <code>getAttributeDefinitions()</code>.
     * <p>
     * <code>ALL</code> indicates that all the definitions are returned.
     */
    ALL
  };

  virtual ~ctkObjectClassDefinition() {}

 /**
  * Return the name of this object class.
  *
  * The name may be localized.
  *
  * @return The name of this object class.
  */
  virtual QString getName() const = 0;

 /**
  * Return the id of this object class.
  *
  * <p>
  * <code>ctkObjectClassDefintion</code> objects share a global namespace in the
  * registry. They share this aspect with LDAP/X.500 attributes. In these
  * standards the OSI Object Identifier (OID) is used to uniquely identify
  * object classes. If such an OID exists, (which can be requested at several
  * standard organisations and many companies already have a node in the
  * tree) it can be returned here. Otherwise, a unique id should be returned
  * which can be a class name combined with a reverse domain name, or generated with a
  * GUID algorithm. Note that all LDAP defined object classes already have an
  * OID associated. It is strongly advised to define the object classes from
  * existing LDAP schemes which will give the OID for free. Many such schemes
  * exist ranging from postal addresses to DHCP parameters.
  *
  * @return The id of this object class.
  */
  virtual QString getID() const = 0;

 /**
  * Return a description of this object class.
  *
  * The description may be localized.
  *
  * @return The description of this object class.
  */
  virtual QString getDescription() const = 0;

 /**
  * Return the attribute definitions for this object class.
  *
  * <p>
  * Return a set of attributes. The filter parameter can distinguish between
  * <code>ALL</code>,<code>REQUIRED</code> or the <code>OPTIONAL</code>
  * attributes.
  *
  * @param filter <code>ALL</code>,<code>REQUIRED</code>,<code>OPTIONAL</code>
  * @return A list of attribute definitions, which is empty if no
  *         attributes are selected
  */
  virtual QList<ctkAttributeDefinitionPtr> getAttributeDefinitions(Filter filter) = 0;

 /**
  * Return a QByteArray object that can be used to create an
  * icon from.
  *
  * <p>
  * Indicate the size and return a QByteAray object containing
  * an icon. The returned icon maybe larger or smaller than the indicated
  * size.
  *
  * <p>
  * The icon may depend on the localization.
  *
  * @param size Requested size of an icon, e.g. a 16x16 pixels icon then size =
  *        16
  * @return A QByteArray holding an icon or an empty QByteArray.
  */
  virtual QByteArray getIcon(int size) const = 0;
};

/**
 * \ingroup Metatype
 */
typedef QSharedPointer<ctkObjectClassDefinition> ctkObjectClassDefinitionPtr;

#endif // CTKOBJECTCLASSDEFINITION_H
