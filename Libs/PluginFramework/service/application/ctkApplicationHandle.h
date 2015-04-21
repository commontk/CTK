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

#ifndef CTKAPPLICATIONHANDLE_H
#define CTKAPPLICATIONHANDLE_H

#include <ctkPluginFrameworkExport.h>

#include <QObject>
#include <QString>

class QVariant;

struct ctkApplicationDescriptor;

/**
 * ApplicationHandle is a service interface which represents an instance
 * of an application. It provides the functionality to query and manipulate the
 * lifecycle state of the represented application instance. It defines constants
 * for the lifecycle states.
 */
struct CTK_PLUGINFW_EXPORT ctkApplicationHandle
{

  /**
   * The property key for the unique identifier (PID) of the application
   * instance.
   */
  static const QString APPLICATION_PID; // = ctkPluginConstants::SERVICE_PID;

  /**
   * The property key for the pid of the corresponding application descriptor.
   */
  static const QString APPLICATION_DESCRIPTOR; // = "application.descriptor";

  /**
   * The property key for the state of this application instance.
   */
  static const QString APPLICATION_STATE; // = "application.state";

  /**
   * The property key for the supports exit value property of this application
   * instance.
   */
  static const QString APPLICATION_SUPPORTS_EXITVALUE; // = "application.supports.exitvalue";

  /**
   * The application instance is running. This is the initial state of a newly
   * created application instance.
   */
  static const QString RUNNING; // = "RUNNING";

  /**
   * The application instance is being stopped. This is the state of the
   * application instance during the execution of the <code>destroy()</code>
   * method.
   */
  static const QString STOPPING; // = "STOPPING";


  virtual ~ctkApplicationHandle() {}

  /**
   * Retrieves the <code>IApplicationDescriptor</code> to which this
   * <code>IApplicationHandle</code> belongs.
   *
   * @return The corresponding <code>IApplicationDescriptor</code>
   */
  virtual ctkApplicationDescriptor* getApplicationDescriptor() const = 0;

  /**
   * Get the state of the application instance.
   *
   * @return the state of the application.
   *
   * @throws ctkIllegalStateException
   *             if the application handle is unregistered
   */
  virtual QString getState() const = 0;

  /**
   * Returns the exit value for the application instance. The timeout
   * specifies how the method behaves when the application has not yet
   * terminated. A negative, zero or positive value may be used.
   * <ul>
   * <li> negative - The method does not wait for termination. If the
   * application has not terminated then an <code>ctkApplicationException</code>
   * is thrown.</li>
   *
   * <li> zero - The method waits until the application terminates.</li>
   *
   * <li> positive - The method waits until the application terminates or the
   * timeout expires. If the timeout expires and the application has not
   * terminated then an <code>ctkApplicationException</code> is thrown.</li>
   * </ul>
   *
   * @param timeout The maximum time in milliseconds to wait for the
   *        application to timeout.
   * @return The exit value for the application instance. The value is
   *         application specific.
   * @throws ctkUnsupportedOperationException If the application model does not
   *         support exit values.
   * @throws ctkApplicationException If the application has not terminated. The
   *         error code will be
   *         ctkApplicationException::APPLICATION_EXITVALUE_NOT_AVAILABLE.
   */
  virtual QVariant getExitValue(long timeout) const = 0;

  /**
   * Returns the unique identifier of this instance. This value is also
   * available as a service property of this application handle's service.pid.
   *
   * @return the unique identifier of the instance
   */
  virtual QString getInstanceId() const = 0;

  /**
   * The application instance's lifecycle state can be influenced by this
   * method. It lets the application instance perform operations to stop
   * the application safely, e.g. saving its state to a permanent storage.
   * <p>
   * The method must check if the lifecycle transition is valid; a STOPPING
   * application cannot be stopped. If it is invalid then the method must
   * exit. Otherwise the lifecycle state of the application instance must be
   * set to STOPPING. Then the destroySpecific() method must be called to
   * perform any application model specific steps for safe stopping of the
   * represented application instance.
   * <p>
   * At the end the <code>ctkApplicationHandle</code> must be unregistered.
   * This method should  free all the resources related to this
   * <code>ctkApplicationHandle</code>.
   * <p>
   * When this method is completed the application instance has already made
   * its operations for safe stopping, the IApplicationHandle has been
   * unregistered and its related resources has been freed. Further calls on
   * this application should not be made because they may have unexpected
   * results.
   *
   * @throws ctkIllegalStateException
   *             if the application handle is unregistered
   */
  virtual void destroy() = 0;

};

Q_DECLARE_INTERFACE(ctkApplicationHandle, "org.commontk.service.application.ApplicationHandle")

#endif // CTKAPPLICATIONHANDLE_H
