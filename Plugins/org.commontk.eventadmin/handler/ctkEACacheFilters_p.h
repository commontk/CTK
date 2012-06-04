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


#ifndef CTKEACACHEFILTERS_P_H
#define CTKEACACHEFILTERS_P_H

#include "ctkEAFilters_p.h"

/**
 * This is an implementation of the <tt>ctkEAFilters</tt> factory that uses a cache in
 * order to speed-up filter creation.
 */
template<class CacheMap>
class ctkEACacheFilters : public ctkEAFilters<ctkEACacheFilters<CacheMap> >
{

private:

  // The cache to use
  CacheMap* const cache;

  // The context of the plugin used to create the Filter objects
  ctkPluginContext* const context;

public:

  /**
   * The constructor of this factory. The cache is used to speed-up filter
   * creation.
   *
   * @param cache The cache to use
   * @param context The context of the plugin used to create the <tt>Filter</tt>
   *      objects
   */
  ctkEACacheFilters(CacheMap* cache,
                    ctkPluginContext* context);

  ~ctkEACacheFilters();

  /**
  * Create a filter for the given filter string or return the TRUE_FILTER in case
  * the string is <tt>null</tt>.
  *
  * @param filter The filter as a string
  * @return The <tt>ctkLDAPSearchFilter</tt> of the filter string or the TRUE_FILTER if the
  *      filter string was <tt>null</tt>
  * @throws ctkInvalidArgumentException if <tt>ctkLDAPSearchFilter()</tt>
  *      throws an <tt>ctkInvalidArgumentException</tt>
  *
  * @see ctkEAFilters#createFilter(const QString&)
  */
  ctkLDAPSearchFilter createFilter(const QString& filter);

};

#include "ctkEACacheFilters.tpp"

#endif // CTKEACACHEFILTERS_P_H
