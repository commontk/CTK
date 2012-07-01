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


#include "ctkException.h"

#include <typeinfo>

#include <QDebug>

// --------------------------------------------------------------------------
ctkException::ctkException(const QString& msg)
  : Msg(msg), NestedException(0)
{
}

// --------------------------------------------------------------------------
ctkException::ctkException(const QString& msg, const ctkException& cause)
  : Msg(msg), NestedException(cause.clone())
{
}

// --------------------------------------------------------------------------
ctkException::ctkException(const ctkException& exc)
  : std::exception(exc), Msg(exc.Msg)
{
  NestedException = exc.NestedException ? exc.NestedException->clone() : 0;
}

// --------------------------------------------------------------------------
ctkException::~ctkException() throw()
{
  delete NestedException;
}

// --------------------------------------------------------------------------
ctkException& ctkException::operator=(const ctkException& exc)
{
  if (&exc != this)
  {
    delete NestedException;
    Msg = exc.Msg;
    NestedException = exc.NestedException ? exc.NestedException->clone() : 0;
  }
  return *this;
}

// --------------------------------------------------------------------------
const ctkException* ctkException::cause() const throw()
{
  return NestedException;
}

// --------------------------------------------------------------------------
void ctkException::setCause(const ctkException& cause)
{
  delete NestedException;
  NestedException = cause.clone();
}

// --------------------------------------------------------------------------
const char *ctkException::name() const throw()
{
  return "ctkException";
}

// --------------------------------------------------------------------------
const char* ctkException::className() const throw()
{
  return typeid(*this).name();
}

// --------------------------------------------------------------------------
const char* ctkException::what() const throw()
{
  static std::string txt;
  txt = std::string(name());
  if (!Msg.isEmpty())
  {
    txt += ": ";
    txt += Msg.toStdString();
  }
  if (NestedException)
  {
    txt +=  std::string("\n  Caused by: ") + NestedException->what();
  }
  return txt.c_str();
}

// --------------------------------------------------------------------------
QString ctkException::message() const throw()
{
  return Msg;
}


// --------------------------------------------------------------------------
ctkException* ctkException::clone() const
{
  return new ctkException(*this);
}

// --------------------------------------------------------------------------
void ctkException::rethrow() const
{
  throw *this;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkException& exc)
{
  dbg << exc.what();
  return dbg.maybeSpace();
}

CTK_IMPLEMENT_EXCEPTION(ctkRuntimeException, ctkException, "ctkRuntimeException")
CTK_IMPLEMENT_EXCEPTION(ctkInvalidArgumentException, ctkRuntimeException, "ctkInvalidArgumentException")
CTK_IMPLEMENT_EXCEPTION(ctkIllegalStateException, ctkRuntimeException, "ctkIllegalStateException")
