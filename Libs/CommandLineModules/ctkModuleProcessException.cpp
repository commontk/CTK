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

#include "ctkModuleProcessException.h"

ctkModuleProcessException::ctkModuleProcessException(const QString& msg, int code,
                                                     QProcess::ExitStatus status)
  : msg(msg), code(code), status(status)
{}

int ctkModuleProcessException::exitCode() const
{
  return code;
}

QProcess::ExitStatus ctkModuleProcessException::exitStatus() const
{
  return status;
}

QString ctkModuleProcessException::message() const
{
  return msg;
}

const char* ctkModuleProcessException::what() const throw()
{
  static std::string strMsg;
  strMsg = msg.toStdString();
  return strMsg.c_str();
}

void ctkModuleProcessException::raise() const
{
  throw *this;
}

ctkModuleProcessException* ctkModuleProcessException::clone() const
{
  return new ctkModuleProcessException(*this);
}
