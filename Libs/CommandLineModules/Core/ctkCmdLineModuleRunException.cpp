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

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const QString& msg)
  : QtConcurrent::Exception(), ctkException(msg)
{
}

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const QString& msg, const ctkCmdLineModuleRunException& cause)
  : QtConcurrent::Exception(), ctkException(msg, cause)
{
}

ctkCmdLineModuleRunException::ctkCmdLineModuleRunException(const ctkCmdLineModuleRunException& o)
  : QtConcurrent::Exception(o), ctkException(o)
{
}

ctkCmdLineModuleRunException::~ctkCmdLineModuleRunException() throw()
{
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
