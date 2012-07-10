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

#include "ctkPluginException.h"

#include <QDebug>

//----------------------------------------------------------------------------
ctkPluginException::ctkPluginException(const QString& msg, const Type& type)
  : ctkRuntimeException(msg),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkPluginException::ctkPluginException(const QString& msg, const Type& type, const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkPluginException::ctkPluginException(const QString& msg, const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(UNSPECIFIED)
{

}

//----------------------------------------------------------------------------
ctkPluginException::ctkPluginException(const ctkPluginException& o)
  : ctkRuntimeException(o), type(o.type)
{

}

//----------------------------------------------------------------------------
ctkPluginException& ctkPluginException::operator=(const ctkPluginException& o)
{
  ctkRuntimeException::operator=(o);
  type = o.type;
  return *this;
}

//----------------------------------------------------------------------------
ctkPluginException::~ctkPluginException() throw()
{
}

//----------------------------------------------------------------------------
const char* ctkPluginException::name() const throw()
{
  return "ctkPluginException";
}

//----------------------------------------------------------------------------
ctkPluginException* ctkPluginException::clone() const
{
  return new ctkPluginException(*this);
}

//----------------------------------------------------------------------------
void ctkPluginException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
ctkPluginException::Type ctkPluginException::getType() const
{
  return type;
}
