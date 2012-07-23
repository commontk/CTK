/*===================================================================
  
BlueBerry Platform

Copyright (c) German Cancer Research Center, 
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without 
even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "ctkCmdLineModuleRunException.h"

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(
    const QString& location, int errorCode, const QString &errorString)
  : QtConcurrent::Exception(),
    ctkException(QString("Running module \"%1\" failed with code %2: %3").arg(location).arg(errorCode).arg(errorString)),
    Location(location), ErrorCode(errorCode), ErrorString(errorString)
{
}

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(
    const QString& location, int errorCode, const QString &errorString,
    const ctkCmdLineModuleRunException& cause)
  : QtConcurrent::Exception(), ctkException(location, cause),
    Location(location), ErrorCode(errorCode), ErrorString(errorString)
{
}

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const ctkCmdLineModuleRunException& o)
  : QtConcurrent::Exception(o), ctkException(o),
    Location(o.Location), ErrorCode(o.ErrorCode), ErrorString(o.ErrorString)
{
}

ctkCmdLineModuleRunException::~ctkCmdLineModuleRunException() throw()
{
}

QString ctkCmdLineModuleRunException::location() const throw()
{
  return Location;
}

int ctkCmdLineModuleRunException::errorCode() const throw()
{
  return ErrorCode;
}

QString ctkCmdLineModuleRunException::errorString() const throw()
{
  return ErrorString;
}

const char* ctkCmdLineModuleRunException::name() const throw()
{
  return "CTK CommandLineModule Run Exception";
}

const char* ctkCmdLineModuleRunException::className() const throw()
{
  return "ctkCmdLineModuleRunException";
}

ctkCmdLineModuleRunException* ctkCmdLineModuleRunException::clone() const
{
  return new ctkCmdLineModuleRunException(*this);
}

void ctkCmdLineModuleRunException::rethrow() const
{
  throw *this;
}

void ctkCmdLineModuleRunException::raise() const
{
  throw *this;
}
