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

#include "ctkLDAPSearchFilter.h"

#include "ctkLDAPExpr_p.h"
#include "ctkServiceReference_p.h"

//----------------------------------------------------------------------------
class ctkLDAPSearchFilterData : public QSharedData
{
public:

  ctkLDAPSearchFilterData()
  {}

  ctkLDAPSearchFilterData(const QString& filter)
    : ldapExpr(filter)
  {}

  ctkLDAPSearchFilterData(const ctkLDAPSearchFilterData& other)
    : QSharedData(other), ldapExpr(other.ldapExpr)
  {}

  ctkLDAPExpr ldapExpr;
};

//----------------------------------------------------------------------------
ctkLDAPSearchFilter::ctkLDAPSearchFilter()
  : d(0)
{
}

//----------------------------------------------------------------------------
ctkLDAPSearchFilter::ctkLDAPSearchFilter(const QString& filter)
  : d(0)
{
  d = new ctkLDAPSearchFilterData(filter);
}

//----------------------------------------------------------------------------
ctkLDAPSearchFilter::ctkLDAPSearchFilter(const ctkLDAPSearchFilter& other)
  : d(other.d)
{
}

//----------------------------------------------------------------------------
ctkLDAPSearchFilter::~ctkLDAPSearchFilter()
{
}

//----------------------------------------------------------------------------
ctkLDAPSearchFilter::operator bool() const
{
  return d;
}

//----------------------------------------------------------------------------
bool ctkLDAPSearchFilter::match(const ctkServiceReference& reference) const
{
  return d->ldapExpr.evaluate(reference.d_func()->getProperties(), true);
}

//----------------------------------------------------------------------------
bool ctkLDAPSearchFilter::match(const ctkDictionary& dictionary) const
{
  return d->ldapExpr.evaluate(dictionary, false);
}

//----------------------------------------------------------------------------
bool ctkLDAPSearchFilter::matchCase(const ctkDictionary& dictionary) const
{
  return d->ldapExpr.evaluate(dictionary, true);
}

//----------------------------------------------------------------------------
QString ctkLDAPSearchFilter::toString() const
{
  return d->ldapExpr.toString();
}

//----------------------------------------------------------------------------
bool ctkLDAPSearchFilter::operator==(const ctkLDAPSearchFilter& other) const
{
  return d->ldapExpr.toString() == other.d->ldapExpr.toString();
}

//----------------------------------------------------------------------------
ctkLDAPSearchFilter& ctkLDAPSearchFilter::operator=(const ctkLDAPSearchFilter& filter)
{
  d = filter.d;

  return *this;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkLDAPSearchFilter& filter)
{
  dbg << filter.toString();
  return dbg.maybeSpace();
}
