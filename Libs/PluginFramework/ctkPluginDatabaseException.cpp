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

#include "ctkPluginDatabaseException.h"

#include <QDebug>

//----------------------------------------------------------------------------
ctkPluginDatabaseException::ctkPluginDatabaseException(const QString& msg, const Type& type)
  : ctkRuntimeException(msg),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkPluginDatabaseException::ctkPluginDatabaseException(const QString& msg, const Type& type,
                                                       const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkPluginDatabaseException::ctkPluginDatabaseException(const QString& msg,
                                                       const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(UNSPECIFIED)
{

}

//----------------------------------------------------------------------------
ctkPluginDatabaseException::ctkPluginDatabaseException(const ctkPluginDatabaseException& o)
  : ctkRuntimeException(o), type(o.type)
{

}

//----------------------------------------------------------------------------
ctkPluginDatabaseException& ctkPluginDatabaseException::operator=(const ctkPluginDatabaseException& o)
{
  ctkRuntimeException::operator=(o);
  type = o.type;
  return *this;
}

//----------------------------------------------------------------------------
ctkPluginDatabaseException::~ctkPluginDatabaseException() throw()
{

}

//----------------------------------------------------------------------------
const char* ctkPluginDatabaseException::name() const throw()
{
  return "ctkPluginDatabaseException";
}

//----------------------------------------------------------------------------
ctkPluginDatabaseException* ctkPluginDatabaseException::clone() const
{
  return new ctkPluginDatabaseException(*this);
}

//----------------------------------------------------------------------------
void ctkPluginDatabaseException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
ctkPluginDatabaseException::Type ctkPluginDatabaseException::getType() const
{
  return type;
}
