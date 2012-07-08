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

#include "ctkServiceException.h"

#include <QDebug>

//----------------------------------------------------------------------------
ctkServiceException::ctkServiceException(const QString& msg, const Type& type)
  : ctkRuntimeException(msg),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkServiceException::ctkServiceException(const QString& msg, const Type& type,
                                         const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(type)
{

}

//----------------------------------------------------------------------------
ctkServiceException::ctkServiceException(const QString& msg, const ctkException& cause)
  : ctkRuntimeException(msg, cause),
    type(UNSPECIFIED)
{

}

//----------------------------------------------------------------------------
ctkServiceException::ctkServiceException(const ctkServiceException& o)
  : ctkRuntimeException(o), type(o.type)
{

}

//----------------------------------------------------------------------------
ctkServiceException& ctkServiceException::operator=(const ctkServiceException& o)
{
  ctkRuntimeException::operator=(o);
  type = o.type;
  return *this;
}

//----------------------------------------------------------------------------
ctkServiceException::~ctkServiceException() throw()
{

}

//----------------------------------------------------------------------------
const char* ctkServiceException::name() const throw()
{
  return "ctkServiceException";
}

//----------------------------------------------------------------------------
ctkServiceException* ctkServiceException::clone() const
{
  return new ctkServiceException(*this);
}

//----------------------------------------------------------------------------
void ctkServiceException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
ctkServiceException::Type ctkServiceException::getType() const
{
  return type;
}

