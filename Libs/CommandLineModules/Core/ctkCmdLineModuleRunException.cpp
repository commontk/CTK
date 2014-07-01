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

#include "ctkCmdLineModuleRunException.h"

#include <QUrl>

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const QUrl &location, int errorCode, const QString &errorString)
  : QtConcurrent::Exception(),
    ctkException(QString("Running module \"%1\" failed with code %2: %3").arg(location.toString()).arg(errorCode).arg(errorString)),
    Location(location), ErrorCode(errorCode), ErrorString(errorString)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const QUrl &location, int errorCode, const QString &errorString,
    const ctkCmdLineModuleRunException& cause)
  : QtConcurrent::Exception(), ctkException(location.toString(), cause),
    Location(location), ErrorCode(errorCode), ErrorString(errorString)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const ctkCmdLineModuleRunException& o)
  : QtConcurrent::Exception(o), ctkException(o),
    Location(o.Location), ErrorCode(o.ErrorCode), ErrorString(o.ErrorString)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException& ctkCmdLineModuleRunException::operator=(const ctkCmdLineModuleRunException& o)
{
  QtConcurrent::Exception::operator=(o);
  ctkException::operator=(o);
  this->Location = o.Location;
  this->ErrorCode = o.ErrorCode;
  this->ErrorString = o.ErrorString;
  return *this;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException::~ctkCmdLineModuleRunException() throw()
{
}

//----------------------------------------------------------------------------
QUrl ctkCmdLineModuleRunException::location() const throw()
{
  return Location;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleRunException::errorCode() const throw()
{
  return ErrorCode;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleRunException::errorString() const throw()
{
  return ErrorString;
}

//----------------------------------------------------------------------------
const char* ctkCmdLineModuleRunException::name() const throw()
{
  return "CTK CommandLineModule Run Exception";
}

//----------------------------------------------------------------------------
const char* ctkCmdLineModuleRunException::className() const throw()
{
  return "ctkCmdLineModuleRunException";
}

//----------------------------------------------------------------------------
ctkCmdLineModuleRunException* ctkCmdLineModuleRunException::clone() const
{
  return new ctkCmdLineModuleRunException(*this);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleRunException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleRunException::raise() const
{
  throw *this;
}
