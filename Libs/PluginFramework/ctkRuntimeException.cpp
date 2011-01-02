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


#include "ctkRuntimeException.h"

ctkRuntimeException::ctkRuntimeException(const QString& msg, const std::exception* cause)
  : std::runtime_error(msg.toStdString())
{
  if (cause)
  {
    this->cause = QString(cause->what());
  }
}

ctkRuntimeException::ctkRuntimeException(const ctkRuntimeException& o)
  : std::runtime_error(o.what()), cause(o.cause)
{

}

ctkRuntimeException& ctkRuntimeException::operator=(const ctkRuntimeException& o)
{
  std::runtime_error::operator=(o);
  cause = o.cause;
  return *this;
}

QString ctkRuntimeException::getCause() const
{
  return cause;
}

void ctkRuntimeException::setCause(const QString& cause) throw(std::logic_error)
{
  if (!this->cause.isEmpty()) throw std::logic_error("The cause for this ctkServiceException instance is already set");

  this->cause = cause;
}

const char* ctkRuntimeException::what() const throw()
{
  static std::string fullMsg;
  fullMsg = std::string(std::runtime_error::what());
  QString causeMsg = getCause();
  if (!causeMsg.isEmpty()) fullMsg += std::string("\n  Caused by: ") + causeMsg.toStdString();

  return fullMsg.c_str();
}
