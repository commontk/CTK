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


#ifndef CTKCONFIGURATIONEXCEPTION_H
#define CTKCONFIGURATIONEXCEPTION_H

#include "ctkRuntimeException.h"

/**
 * An exception class to inform the Configuration Admin service
 * of problems with configuration data.
 */
class Q_DECL_EXPORT ctkConfigurationException : public ctkRuntimeException
{

public:

  /**
   * Create a {@code ctkConfigurationException} object.
   *
   * @param property name of the property that caused the problem,
   *        {@code null} if no specific property was the cause
   * @param reason reason for failure
   * @param cause The cause of this exception.
   */
  ctkConfigurationException(const QString& property, const QString& reason,
                            const std::exception* cause = 0);

  ctkConfigurationException(const ctkConfigurationException& o);
  ctkConfigurationException& operator=(const ctkConfigurationException& o);

  ~ctkConfigurationException() throw() { }

  /**
   * Return the property name that caused the failure or a null QString.
   *
   * @return name of property or null if no specific property caused the
   *         problem
   */
  QString getProperty() const;

  /**
   * Return the reason for this exception.
   *
   * @return reason of the failure
   */
  QString getReason() const;

private:

  QString property;
  QString reason;
};

CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkConfigurationException& exc);

#endif // CTKCONFIGURATIONEXCEPTION_H
