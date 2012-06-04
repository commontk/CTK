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

#ifndef CTKLDAPSEARCHFILTER_H
#define CTKLDAPSEARCHFILTER_H

#include "ctkPluginFrameworkExport.h"

#include "ctkServiceReference.h"
#include "ctkDictionary.h"

#include <QSharedDataPointer>
#include <QDebug>

class ctkLDAPSearchFilterData;

/**
 * \ingroup PluginFramework
 *
 * An <a href="http://www.ietf.org/rfc/rfc1960.txt">RFC 1960</a>-based Filter.
 *
 * <p>
 * A <code>ctkLDAPSearchFilter</code> can be used numerous times to determine if the match
 * argument matches the filter string that was used to create the <code>ctkLDAPSearchFilter</code>.
 * <p>
 * Some examples of LDAP filters are:
 *
 * \verbatim
 * "(cn=Babs Jensen)"
 * "(!(cn=Tim Howes))"
 * "(&(" + ctkPluginConstants::OBJECTCLASS + "=Person)(|(sn=Jensen)(cn=Babs J*)))"
 *  "(o=univ*of*mich*)"
 * \endverbatim
 *
 * \see "CTK Wiki for a description of the filter string syntax." TODO!
 * \remarks This class is thread safe.
 */
class CTK_PLUGINFW_EXPORT ctkLDAPSearchFilter {

public:

  /**
   * Creates in invalid <code>ctkLDAPSearchFilter</code> object.
   * Test the validity by using the boolean conversion operator.
   *
   * <p>
   * Calling methods on an invalid <code>ctkLDAPSearchFilter</code>
   * will result in undefined behavior.
   */
  ctkLDAPSearchFilter();

  /**
   * Creates a <code>ctkLDAPSearchFilter</code> object. This <code>ctkLDAPSearchFilter</code>
   * object may be used to match a <code>ctkServiceReference</code> object or a
   * <code>ctkDictionary</code> object.
   *
   * <p>
   * If the filter cannot be parsed, an ctkInvalidArgumentException will be
   * thrown with a human readable message where the filter became unparsable.
   *
   * @param filter The filter string.
   * @return A <code>ctkLDAPSearchFilter</code> object encapsulating the filter string.
   * @throws ctkInvalidArgumentException If <code>filter</code> contains an invalid
   *         filter string that cannot be parsed.
   * @see "Framework specification for a description of the filter string syntax." TODO!
   */
  ctkLDAPSearchFilter(const QString& filter);

  ctkLDAPSearchFilter(const ctkLDAPSearchFilter& other);

  ~ctkLDAPSearchFilter();

  operator bool() const;

  /**
   * Filter using a service's properties.
   * <p>
   * This <code>ctkLDAPSearchFilter</code> is executed using the keys and values of the
   * referenced service's properties. The keys are looked up in a case
   * insensitive manner.
   *
   * @param reference The reference to the service whose properties are used
   *        in the match.
   * @return <code>true</code> if the service's properties match this
   *         <code>ctkLDAPSearchFilter</code> <code>false</code> otherwise.
   */
  bool match(const ctkServiceReference& reference) const;

  /**
   * Filter using a <code>ctkDictionary</code> with case insensitive key lookup. This
   * <code>ctkLDAPSearchFilter</code> is executed using the specified <code>ctkDictionary</code>'s keys
   * and values. The keys are looked up in a case insensitive manner.
   *
   * @param dictionary The <code>ctkDictionary</code> whose key/value pairs are used
   *        in the match.
   * @return <code>true</code> if the <code>ctkDictionary</code>'s values match this
   *         filter; <code>false</code> otherwise.
   */
  bool match(const ctkDictionary& dictionary) const;

  /**
   * Filter using a <code>ctkDictionary</code>. This <code>ctkLDAPSearchFilter</code> is executed using
   * the specified <code>ctkDictionary</code>'s keys and values. The keys are looked
   * up in a normal manner respecting case.
   *
   * @param dictionary The <code>ctkDictionary</code> whose key/value pairs are used
   *        in the match.
   * @return <code>true</code> if the <code>ctkDictionary</code>'s values match this
   *         filter; <code>false</code> otherwise.
   */
  bool matchCase(const ctkDictionary& dictionary) const;

  /**
   * Returns this <code>ctkLDAPSearchFilter</code>'s filter string.
   * <p>
   * The filter string is normalized by removing whitespace which does not
   * affect the meaning of the filter.
   *
   * @return This <code>ctkLDAPSearchFilter</code>'s filter string.
   */
  QString toString() const;

  /**
   * Compares this <code>ctkLDAPSearchFilter</code> to another <code>ctkLDAPSearchFilter</code>.
   *
   * <p>
   * This implementation returns the result of calling
   * <code>this->toString() == other.toString()</code>.
   *
   * @param other The object to compare against this <code>ctkLDAPSearchFilter</code>.
   * @return Returns the result of calling
   *         <code>this->toString() == other.toString()</code>.
   */
  bool operator==(const ctkLDAPSearchFilter& other) const;

  ctkLDAPSearchFilter& operator=(const ctkLDAPSearchFilter& filter);

protected:

  QSharedDataPointer<ctkLDAPSearchFilterData> d;

};

/**
 * \ingroup PluginFramework
 */
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkLDAPSearchFilter& filter);

#endif // CTKLDAPSEARCHFILTER_H
