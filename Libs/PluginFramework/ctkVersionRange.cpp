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

#include "ctkVersionRange_p.h"

#include <ctkException.h>

//----------------------------------------------------------------------------
ctkVersionRange ctkVersionRange::defaultVersionRange()
{
  static ctkVersionRange defaultVR;
  return defaultVR;
}

//----------------------------------------------------------------------------
ctkVersionRange::ctkVersionRange(const QString& vr)
{
  bool op = vr.startsWith("(");
  bool ob = vr.startsWith("[");

  if (op || ob) {
    bool cp = vr.endsWith(")");
    bool cb = vr.endsWith("]");
    int comma = vr.indexOf(',');

    if (comma > 0 && (cp || cb))
    {
      low = ctkVersion(vr.mid(1, comma-1).trimmed());
      high = ctkVersion(vr.mid(comma+1, vr.length()-comma-2).trimmed());
      lowIncluded = ob;
      highIncluded = cb;
    }
    else
    {
      throw ctkInvalidArgumentException("Illegal version range: " + vr);
    }
  }
  else
  {
    low = ctkVersion(vr);
    high = ctkVersion();
    lowIncluded = true;
    highIncluded = false;
  }
}

//----------------------------------------------------------------------------
ctkVersionRange::ctkVersionRange()
{
  low = ctkVersion(ctkVersion::emptyVersion());
  high = ctkVersion();
  lowIncluded = true;
  highIncluded = false;
}

//----------------------------------------------------------------------------
ctkVersionRange::~ctkVersionRange()
{
}

//----------------------------------------------------------------------------
bool ctkVersionRange::isSpecified() const
{
  return !(*this == defaultVersionRange());
}

//----------------------------------------------------------------------------
bool ctkVersionRange::withinRange(const ctkVersion& ver) const
{
  if (*this == defaultVersionRange())
  {
    return true;
  }
  int c = low.compare(ver);

  if (c < 0 || (c == 0 && lowIncluded))
  {
    if (high.isUndefined())
    {
      return true;
    }
    c = high.compare(ver);
    return c > 0 || (c == 0 && highIncluded);
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkVersionRange::withinRange(const ctkVersionRange& range) const
{
  if (*this == range) {
    return true;
  }
  int c = low.compare(range.low);

  if (c < 0 || (c == 0 && lowIncluded == range.lowIncluded))
  {
    if (high.isUndefined())
    {
      return true;
    }
    c = high.compare(range.high);
    return c > 0 || (c == 0 && highIncluded == range.highIncluded);
  }
  return false;
}

//----------------------------------------------------------------------------
int ctkVersionRange::compare(const ctkVersionRange& obj) const
{
  return low.compare(obj.low);
}

//----------------------------------------------------------------------------
QString ctkVersionRange::toString() const
{
  if (!high.isUndefined())
  {
    QString res;
    if (lowIncluded)
    {
      res += '[';
    }
    else
    {
      res += '(';
    }
    res += low.toString() + "," + high.toString();
    if (highIncluded)
    {
      res += ']';
    }
    else
    {
      res += ')';
    }
    return res;
  }
  else
  {
    return low.toString();
  }
}

//----------------------------------------------------------------------------
bool ctkVersionRange::operator==(const ctkVersionRange& r) const
{
  if (low == r.low)
  {
    if (!high.isUndefined())
    {
      return (high == r.high)  &&
        (lowIncluded == r.lowIncluded) &&
        (highIncluded == r.highIncluded);
    }
    else
    {
      return true;
    }
  }
  return false;
}
