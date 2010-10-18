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


ctkPluginDatabaseException::ctkPluginDatabaseException(const QString& msg, const Type& type, const std::exception& cause)
  : std::runtime_error(msg.toStdString()),
    type(type), cause(cause)
{

}

ctkPluginDatabaseException::ctkPluginDatabaseException(const QString& msg, const std::exception& cause)
  : std::runtime_error(msg.toStdString()),
    type(UNSPECIFIED), cause(cause)
{

}

ctkPluginDatabaseException::ctkPluginDatabaseException(const ctkPluginDatabaseException& o)
  : std::runtime_error(o.what()),
    type(o.type), cause(o.cause)
{

}

ctkPluginDatabaseException& ctkPluginDatabaseException::operator=(const ctkPluginDatabaseException& o)
{
  std::runtime_error::operator=(o);
  type = o.type;
  cause = o.cause;
  return *this;
}

std::exception ctkPluginDatabaseException::getCause() const
{
  return cause;
}

void ctkPluginDatabaseException::setCause(const std::exception& cause) throw(std::logic_error)
{
  if (!cause.what()) throw std::logic_error("The cause for this ctkPluginDatabaseException instance is already set");

  this->cause = cause;
}

ctkPluginDatabaseException::Type ctkPluginDatabaseException::getType() const
{
  return type;
}


QDebug operator<<(QDebug dbg, const ctkPluginDatabaseException& exc)
{
  dbg << "ctkPluginDatabaseException:" << exc.what();

  const char* causeMsg = exc.getCause().what();
  if (causeMsg) dbg << "  Caused by:" << causeMsg;

  return dbg.maybeSpace();
}

