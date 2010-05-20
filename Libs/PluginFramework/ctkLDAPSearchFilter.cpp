/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkLDAPSearchFilter.h"


  class ctkLDAPSearchFilterPrivate {
  public:

    ctkLDAPSearchFilterPrivate()
      : ref(1)
    {}

    QAtomicInt ref;

  };

  ctkLDAPSearchFilter::ctkLDAPSearchFilter(const QString& filter)
    : d(new ctkLDAPSearchFilterPrivate())
  {

  }

  ctkLDAPSearchFilter::ctkLDAPSearchFilter(const ctkLDAPSearchFilter& filter)
    : d(filter.d)
  {
    d->ref.ref();
  }

  ctkLDAPSearchFilter::~ctkLDAPSearchFilter()
  {
    if (!d->ref.deref())
      delete d;
  }

  bool ctkLDAPSearchFilter::match(const Dictionary& dictionary) const
  {
    return true;
  }

  bool ctkLDAPSearchFilter::matchCase(const Dictionary& dictionary) const
  {
    return true;
  }

  bool ctkLDAPSearchFilter::operator==(const ctkLDAPSearchFilter& other) const
  {
    // TODO
    return true;
  }

  ctkLDAPSearchFilter& ctkLDAPSearchFilter::operator=(const ctkLDAPSearchFilter& filter)
  {
    if (d != filter.d)
    {
      if (!d->ref.deref())
        delete d;

      d = filter.d;
      d->ref.ref();
    }

    return *this;


}
