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

#include "ctkException.h"

#include <ctkPluginFrameworkExport.h>

/**
 * \ingroup PluginFramework
 */
class CTK_PLUGINFW_EXPORT ctkPluginDatabaseException : public ctkRuntimeException
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

  ctkPluginDatabaseException(const QString& msg, const Type& type = UNSPECIFIED);
  ctkPluginDatabaseException(const QString& msg, const Type& type, const ctkException& cause);
  ctkPluginDatabaseException(const QString& msg, const ctkException& cause);

  ctkPluginDatabaseException(const ctkPluginDatabaseException& o);
  ctkPluginDatabaseException& operator=(const ctkPluginDatabaseException& o);

  ~ctkPluginDatabaseException() throw();

  /**
   * @see ctkException::name()
   */
  const char* name() const throw();

  /**
   * @see ctkException::clone()
   */
  ctkPluginDatabaseException* clone() const;

  /**
   * @see ctkException::rethrow()
   */
  void rethrow() const;

  Type getType() const;

private:

  Type type;

};

#endif // CTKPLUGINDATABASEEXCEPTION_H
