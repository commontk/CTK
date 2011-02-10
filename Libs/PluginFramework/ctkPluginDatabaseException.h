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

#ifndef CTKPLUGINDATABASEEXCEPTION_H
#define CTKPLUGINDATABASEEXCEPTION_H

#include "ctkRuntimeException.h"

/**
 * \ingroup PluginFramework
 */
class Q_DECL_EXPORT ctkPluginDatabaseException : public ctkRuntimeException
{
public:

  enum Type {
    UNSPECIFIED,
    DB_CONNECTION_INVALID,
    DB_NOT_OPEN_ERROR,
    DB_NOT_FOUND_ERROR,
    DB_CREATE_DIR_ERROR,
    DB_WRITE_ERROR,
    DB_FILE_INVALID,
    DB_SQL_ERROR
  };

  ctkPluginDatabaseException(const QString& msg, const Type& type = UNSPECIFIED, const std::exception* cause = 0);
  ctkPluginDatabaseException(const QString& msg, const std::exception* cause);

  ctkPluginDatabaseException(const ctkPluginDatabaseException& o);
  ctkPluginDatabaseException& operator=(const ctkPluginDatabaseException& o);

  Type getType() const;

private:

  Type type;

};

/**
 * \ingroup PluginFramework
 */
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkPluginDatabaseException& exc);

#endif // CTKPLUGINDATABASEEXCEPTION_H
