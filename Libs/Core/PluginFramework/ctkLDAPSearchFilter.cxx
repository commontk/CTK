/*
 * ctkLDAPSearchFilter.cxx
 *
 *  Created on: Mar 28, 2010
 *      Author: zelzer
 */

#include "ctkLDAPSearchFilter.h"

namespace ctk {

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


}
