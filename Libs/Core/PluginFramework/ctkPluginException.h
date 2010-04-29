/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#ifndef CTKPLUGINEXCEPTION_H
#define CTKPLUGINEXCEPTION_H

#include <stdexcept>

#include <QString>
#include <QDebug>

#include "CTKCoreExport.h"

namespace ctk {

  class CTK_CORE_EXPORT PluginException : public std::runtime_error
  {
  public:

    enum Type {
      /**
       * No exception type is unspecified.
       */
      UNSPECIFIED,
      /**
       * The operation was unsupported.
       */
      UNSUPPORTED_OPERATION,
      /**
       * The operation was invalid.
       */
      INVALID_OPERATION,
      /**
       * The plugin manifest contains errors.
       */
      MANIFEST_ERROR,
      /**
       * The plugin was not resolved.
       */
      RESOLVE_ERROR,
      /**
       * The plugin activator was in error.
       */
      ACTIVATOR_ERROR,
      /**
       * The operation failed due to insufficient permissions.
       */
      SECURITY_ERROR,
      /**
       * The operation failed to complete the requested lifecycle state change.
       */
      STATECHANGE_ERROR,
      /**
       * The install or update operation failed because another
       * already installed plugin has the same symbolic name and version.
       */
      DUPLICATE_BUNDLE_ERROR
    };

    PluginException(const QString& msg, const Type& type = UNSPECIFIED, const std::exception& cause = std::exception());
    PluginException(const QString& msg, const std::exception& cause);

    PluginException(const PluginException& o);
    PluginException& operator=(const PluginException& o);

    ~PluginException() throw() {}

    std::exception getCause() const;
    void setCause(const std::exception&) throw(std::logic_error);
    Type getType() const;

    const char* what() const throw();

  private:

    QString msg;
    Type type;
    std::exception cause;

  };

}

CTK_CORE_EXPORT QDebug operator<<(QDebug dbg, const ctk::PluginException& exc);

#endif // CTKPLUGINEXCEPTION_H
