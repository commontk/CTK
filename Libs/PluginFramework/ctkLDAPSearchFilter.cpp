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


  class LDAPSearchFilterPrivate {
  public:

    LDAPSearchFilterPrivate()
      : ref(1)
    {}

    QAtomicInt ref;

  };

  LDAPSearchFilter::LDAPSearchFilter(const QString& filter)
    : d(new LDAPSearchFilterPrivate())
  {

  }

  LDAPSearchFilter::LDAPSearchFilter(const LDAPSearchFilter& filter)
    : d(filter.d)
  {
    d->ref.ref();
  }

  LDAPSearchFilter::~LDAPSearchFilter()
  {
    if (!d->ref.deref())
      delete d;
  }

  bool LDAPSearchFilter::match(const Dictionary& dictionary) const
  {
    return true;
  }

  bool LDAPSearchFilter::matchCase(const Dictionary& dictionary) const
  {
    return true;
  }

  bool LDAPSearchFilter::operator==(const LDAPSearchFilter& other) const
  {
    // TODO
    return true;
  }

  LDAPSearchFilter& LDAPSearchFilter::operator=(const LDAPSearchFilter& filter)
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
