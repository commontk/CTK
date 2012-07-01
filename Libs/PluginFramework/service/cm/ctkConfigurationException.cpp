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


#include "ctkConfigurationException.h"

#include <QDebug>

//----------------------------------------------------------------------------
ctkConfigurationException::ctkConfigurationException(const QString& property, const QString& reason)
  : ctkRuntimeException(property + " : " + reason),
    property(property), reason(reason)
{

}

//----------------------------------------------------------------------------
ctkConfigurationException::ctkConfigurationException(const QString& property, const QString& reason,
                                                     const ctkException& cause)
  : ctkRuntimeException(property + " : " + reason, cause),
    property(property), reason(reason)
{

}

//----------------------------------------------------------------------------
ctkConfigurationException::ctkConfigurationException(const ctkConfigurationException& o)
  : ctkRuntimeException(o), property(o.property), reason(o.reason)
{

}

//----------------------------------------------------------------------------
ctkConfigurationException& ctkConfigurationException::operator=(const ctkConfigurationException& o)
{
  ctkRuntimeException::operator =(o);
  property = o.property;
  reason = o.reason;
  return *this;
}

//----------------------------------------------------------------------------
ctkConfigurationException::~ctkConfigurationException() throw()
{
}

//----------------------------------------------------------------------------
const char* ctkConfigurationException::name() const throw()
{
  return "ctkConfigurationException";
}

//----------------------------------------------------------------------------
ctkConfigurationException* ctkConfigurationException::clone() const
{
  return new ctkConfigurationException(*this);
}

//----------------------------------------------------------------------------
void ctkConfigurationException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
QString ctkConfigurationException::getProperty() const
{
  return property;
}

//----------------------------------------------------------------------------
QString ctkConfigurationException::getReason() const
{
  return reason;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkConfigurationException& exc)
{
  dbg << "ctkConfigurationException:" << exc.what();
  return dbg.maybeSpace();
}
