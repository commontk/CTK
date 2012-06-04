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


#ifndef CTKLOGENTRY_H
#define CTKLOGENTRY_H

#include <QSharedPointer>
#include <QDateTime>
#include <QMetaType>

#include <ctkServiceReference.h>

class ctkRuntimeException;


/**
 * \ingroup LogService
 *
 * Provides methods to access the information contained in an individual Log
 * Service log entry.
 *
 * <p>
 * A <code>ctkLogEntry</code> object may be acquired from the
 * <code>ctkLogReaderService#getLog()</code> method or by registering a
 * <code>ctkLogListener</code> object.
 *
 * @remarks This class is thread safe.
 * @see ctkLogReaderService#getLog()
 * @see ctkLogListener
 */
struct ctkLogEntry
{
  virtual ~ctkLogEntry() {}

  /**
   * Returns the plugin that created this <code>ctkLogEntry</code> object.
   *
   * @return The plugin that created this <code>ctkLogEntry</code> object;
   *         null if no plugins is associated with this
   *         <code>ctkLogEntry</code> object.
   */
  virtual QSharedPointer<ctkPlugin> getPlugin() const = 0;

  /**
   * Returns the <code>ctkServiceReference</code> object for the service associated
   * with this <code>ctkLogEntry</code> object.
   *
   * @return <code>ctkServiceReference</code> object for the service associated
   *         with this <code>ctkLogEntry</code> object; A default constructed object if no
   *         <code>ctkServiceReference</code> object was provided.
   */
  virtual ctkServiceReference getServiceReference() const = 0;

  /**
   * Returns the severity level of this <code>ctkLogEntry</code> object.
   *
   * <p>
   * This is one of the severity levels defined by the <code>ctkLogService</code>
   * interface.
   *
   * @return Severity level of this <code>ctkLogEntry</code> object.
   *
   * @see ctkLogService#LOG_ERROR
   * @see ctkLogService#LOG_WARNING
   * @see ctkLogService#LOG_INFO
   * @see ctkLogService#LOG_DEBUG
   */
  virtual int getLevel() const = 0;

  /**
   * Returns the human readable message associated with this <code>ctkLogEntry</code>
   * object.
   *
   * @return <code>QString</code> containing the message associated with this
   *         <code>ctkLogEntry</code> object.
   */
  virtual QString getMessage() const = 0;

  /**
   * Returns the absolute file name of the source file with which this
   * <code>ctkLogEntry</code> is associated.
   *
   * @return The source file name or an empty string if no information
   *         about the file name is available.
   */
  virtual QString getFileName() const = 0;

  /**
   * Returns the function name of the calling function with which this
   * <code>ctkLogEntry</code> is associated.
   *
   * @return The function name or an empty string if no information
   *         about the function is available.
   */
  virtual QString getFunctionName() const = 0;

  /**
   * Returns the line number in the source file with which this
   * <code>ctkLogEntry</code> is associated.
   *
   * @return The line number (a positive integer) or 0 if no information
   *         about the line number is available.
   */
  virtual int getLineNumber() const = 0;

  /**
   * Returns the exception object associated with this <code>ctkLogEntry</code>
   * object.
   *
   * <p>
   * In some implementations, the returned exception may not be the original
   * exception. For example, STL exceptions associated with log entries may be wrapped
   * in a derived ctkRuntimeException. The returned object will attempt to provide as much
   * information as possible from the original exception object.
   *
   * @return <code>ctkRuntimeException</code> object of the exception associated with this
   *         <code>ctkLogEntry</code>; <code>null</code> if no exception is
   *         associated with this <code>ctkLogEntry</code> object.
   */
  virtual ctkRuntimeException* getException() const = 0;

  /**
   * Returns the value of <code>QDateTime::currentDateTime()</code> at the time this
   * <code>ctkLogEntry</code> object was created.
   *
   * @return The system time when this <code>ctkLogEntry</code>
   *         object was created.
   * @see "QDateTime::currentDateTime()"
   */
  virtual QDateTime getTime() const = 0;
};

/**
 * \ingroup LogService
 * @{
 */

typedef QSharedPointer<ctkLogEntry> ctkLogEntryPtr;

Q_DECLARE_METATYPE(ctkLogEntryPtr)

/** @}*/

#endif // CTKLOGENTRY_H
