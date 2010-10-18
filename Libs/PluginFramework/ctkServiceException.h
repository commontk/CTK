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


#ifndef CTKSERVICEEXCEPTION_H
#define CTKSERVICEEXCEPTION_H

#include <stdexcept>

#include <QString>

#include "CTKPluginFrameworkExport.h"


class CTK_PLUGINFW_EXPORT ctkServiceException : public std::runtime_error
{
public:

  enum Type {
    /**
     * No exception type is unspecified.
     */
    UNSPECIFIED			= 0,
    /**
     * The service has been unregistered.
     */
    UNREGISTERED		= 1,
    /**
     * The service factory produced an invalid service object.
     */
    FACTORY_ERROR		= 2,
    /**
     * The service factory threw an exception.
     */
    FACTORY_EXCEPTION	= 3,
    /**
     * The exception is a subclass of ctkServiceException. The subclass should be
     * examined for the type of the exception.
     */
    SUBCLASSED			= 4,
    /**
     * An error occurred invoking a remote service.
     */
    REMOTE 				= 5
  };

  ctkServiceException(const QString& msg, const Type& type = UNSPECIFIED, const std::exception& cause = std::exception());
  ctkServiceException(const QString& msg, const std::exception& cause);

  ctkServiceException(const ctkServiceException& o);
  ctkServiceException& operator=(const ctkServiceException& o);

  ~ctkServiceException() throw() {}

  std::exception getCause() const;
  void setCause(const std::exception&) throw(std::logic_error);
  Type getType() const;


private:

  Type type;
  std::exception cause;

};


CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkServiceException& exc);

#endif // CTKSERVICEEXCEPTION_H
