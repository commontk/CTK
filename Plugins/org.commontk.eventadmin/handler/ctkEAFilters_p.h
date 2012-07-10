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


#ifndef CTKEAFILTERS_P_H
#define CTKEAFILTERS_P_H

#include <ctkLDAPSearchFilter.h>
#include <service/event/ctkEventConstants.h>

/**
 * The factory for <tt>ctkLDAPSearchFilter</tt> objects. Additionally, two null filter objects
 * are provided that either always return <tt>true</tt> or <tt>false</tt>,
 * respectively.
 */
template<class Impl>
struct ctkEAFilters
{
  /**
   * A null filter object that matches any given service reference.
   */
  static const ctkLDAPSearchFilter TRUE_FILTER;

  /**
   * Create a filter for the given filter string or return the TRUE_FILTER in case
   * the string is <tt>null</tt>.
   *
   * @param filter The filter as a string
   * @return The <tt>ctkLDAPSearchFilter</tt> of the filter string or the TRUE_FILTER if the
   *      filter string was null
   * @throws ctkInvalidArgumentException if <tt>ctkLDAPSearchFilter()</tt>
   *      throws an <tt>ctkInvalidArgumentException</tt>
   */
  ctkLDAPSearchFilter createFilter(const QString& filter)
  {
    return static_cast<Impl*>(this)->createFilter(filter);
  }

  virtual ~ctkEAFilters() {}
};

template<class Impl>
const ctkLDAPSearchFilter ctkEAFilters<Impl>::TRUE_FILTER(QString("(") + ctkEventConstants::EVENT_TOPIC + "=*)");

#endif // CTKEAFILTERS_P_H
