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


ctkServiceException::ctkServiceException(const QString& msg, const Type& type, const std::exception* cause)
  : std::runtime_error(msg.toStdString()),
    type(type)
{
  if (cause)
  {
    this->cause = QString(cause->what());
  }
}

ctkServiceException::ctkServiceException(const QString& msg, const std::exception* cause)
  : std::runtime_error(msg.toStdString()),
    type(UNSPECIFIED)
{
  if (cause)
  {
    this->cause = QString(cause->what());
  }
}

ctkServiceException::ctkServiceException(const ctkServiceException& o)
  : std::runtime_error(o.what()), type(o.type), cause(o.cause)
{

}

ctkServiceException& ctkServiceException::operator=(const ctkServiceException& o)
{
  std::runtime_error::operator=(o);
  type = o.type;
  cause = o.cause;
  return *this;
}

QString ctkServiceException::getCause() const
{
  return cause;
}

void ctkServiceException::setCause(const QString& cause) throw(std::logic_error)
{
  if (!this->cause.isEmpty()) throw std::logic_error("The cause for this ctkServiceException instance is already set");

  this->cause = cause;
}

ctkServiceException::Type ctkServiceException::getType() const
{
  return type;
}

const char* ctkServiceException::what() const throw()
{
  static std::string fullMsg;
  fullMsg = std::string(std::runtime_error::what());
  QString causeMsg = getCause();
  if (!causeMsg.isEmpty()) fullMsg += std::string("\n  Caused by: ") + causeMsg.toStdString();

  return fullMsg.c_str();
}

QDebug operator<<(QDebug dbg, const ctkServiceException& exc)
{
  dbg << "ctkServiceException:" << exc.what();

  return dbg.maybeSpace();
}
