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

#ifndef CTKAPPLICATIONEXCEPTION_H
#define CTKAPPLICATIONEXCEPTION_H

#include <ctkPluginFrameworkExport.h>

#include <ctkException.h>

/**
 * This exception is used to indicate problems related to application lifecycle
 * management.
 *
 * <code>ctkApplicationException</code> object is created by the Application Admin
 * to denote an exception condition in the lifecycle of an application.
 * <code>ctkApplicationException</code>s should not be created by developers. <br/>
 * <code>ctkApplicationException</code>s are associated with an error code. This
 * code describes the type of problem reported in this exception. The possible
 * codes are:
 * <ul>
 * <li> APPLICATION_LOCKED - The application couldn't be launched
 * because it is locked.</li>
 * <li> APPLICATION_NOT_LAUNCHABLE - The application is not in
 * launchable state.</li>
 * <li> APPLICATION_INTERNAL_ERROR - An exception was thrown by the
 * application or its container during launch.</li>
 * <li> APPLICATION_SCHEDULING_FAILED - The scheduling of an application
 * failed.
 * <li> APPLICATION_DUPLICATE_SCHEDULE_ID - The application scheduling
 * failed because the specified identifier is already in use.
 * <li> APPLICATION_EXITVALUE_NOT_AVAILABLE - The exit value is not
 * available for an application instance because the instance has not
 * terminated.
 * <li> APPLICATION_INVALID_STARTUP_ARGUMENT - One of the specified
 * startup arguments is invalid, for example its type is not permitted.
 * </ul>
 */
class CTK_PLUGINFW_EXPORT ctkApplicationException : public ctkException
{

private:

  int errorCode;

public:

  /**
   * The application couldn't be launched because it is locked.
   */
  static const int APPLICATION_LOCKED; // = 0x01;

  /**
   * The application is not in launchable state, it's
   * {@link ApplicationDescriptor#APPLICATION_LAUNCHABLE}
   * attribute is false.
   */
  static const int APPLICATION_NOT_LAUNCHABLE; // = 0x02;

  /**
   * An exception was thrown by the application or the corresponding container
   * during launch. The exception is available from <code>getCause()</code>.
   */
  static const int APPLICATION_INTERNAL_ERROR; // = 0x03;

  /**
   * The application schedule could not be created due to some internal error
   * (for example, the schedule information couldn't be saved due to some
   * storage error).
   */
  static const int APPLICATION_SCHEDULING_FAILED; // = 0x04;

  /**
   * The application scheduling failed because the specified identifier
   * is already in use.
   */
  static const int APPLICATION_DUPLICATE_SCHEDULE_ID; // = 0x05;

  /**
   * The exit value is not available for an application instance because the
   * instance has not terminated.
   */
  static const int APPLICATION_EXITVALUE_NOT_AVAILABLE; // = 0x06;

  /**
   * One of the specified startup arguments is invalid, for example its
   * type is not permitted.
   */
  static const int APPLICATION_INVALID_STARTUP_ARGUMENT; // = 0x07;

  /**
   * Creates an <code>ApplicationException</code> with the specified error code.
   * @param errorCode The code of the error
   */
  ctkApplicationException(int errorCode);

  /**
   * Creates a <code>ApplicationException</code> that wraps another exception.
   *
   * @param errorCode The code of the error
   * @param cause The cause of this exception.
   */
  ctkApplicationException(int errorCode, const ctkException& cause);

  /**
   * Creates an <code>ApplicationException</code> with the specified error code.
   * @param errorCode The code of the error
   * @param message The associated message
   */
  ctkApplicationException(int errorCode, const QString& message);

  /**
   * Creates a <code>ApplicationException</code> that wraps another exception.
   *
   * @param errorCode The code of the error
   * @param message The associated message.
   * @param cause The cause of this exception.
   */
  ctkApplicationException(int errorCode, const QString& message, const ctkException& cause);

  ~ctkApplicationException() throw();

  const char* name() const throw();

  ctkApplicationException* clone() const;

  void rethrow() const;

  /**
   * Returns the error code associated with this exception.
   *
   * @return The error code of this exception.
   */
  int GetErrorCode() const;
};

#endif // CTKAPPLICATIONEXCEPTION_H
