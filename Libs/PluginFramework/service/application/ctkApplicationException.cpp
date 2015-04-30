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

#include "ctkApplicationException.h"

const int ctkApplicationException::APPLICATION_LOCKED	= 0x01;
const int ctkApplicationException::APPLICATION_NOT_LAUNCHABLE = 0x02;
const int ctkApplicationException::APPLICATION_INTERNAL_ERROR = 0x03;
const int ctkApplicationException::APPLICATION_SCHEDULING_FAILED = 0x04;
const int ctkApplicationException::APPLICATION_DUPLICATE_SCHEDULE_ID = 0x05;
const int ctkApplicationException::APPLICATION_EXITVALUE_NOT_AVAILABLE = 0x06;
const int ctkApplicationException::APPLICATION_INVALID_STARTUP_ARGUMENT = 0x07;


//----------------------------------------------------------------------------
ctkApplicationException::ctkApplicationException(int errorCode)
  : ctkException("")
  , errorCode(errorCode)
{
}

//----------------------------------------------------------------------------
ctkApplicationException::ctkApplicationException(int errorCode, const ctkException& cause)
  : ctkException("", cause)
  , errorCode(errorCode)
{
}

//----------------------------------------------------------------------------
ctkApplicationException::ctkApplicationException(int errorCode, const QString& message)
  : ctkException(message)
  , errorCode(errorCode)
{
}

//----------------------------------------------------------------------------
ctkApplicationException::ctkApplicationException(int errorCode, const QString& message, const ctkException& cause)
  : ctkException(message, cause)
  , errorCode(errorCode)
{
}

//----------------------------------------------------------------------------
ctkApplicationException::~ctkApplicationException() throw()
{

}

//----------------------------------------------------------------------------
const char* ctkApplicationException::name() const throw()
{
  return "Application Exception";
}

//----------------------------------------------------------------------------
ctkApplicationException* ctkApplicationException::clone() const
{
  return new ctkApplicationException(*this);
}

//----------------------------------------------------------------------------
void ctkApplicationException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
int ctkApplicationException::GetErrorCode() const
{
  return errorCode;
}
