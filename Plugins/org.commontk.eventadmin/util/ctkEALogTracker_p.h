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


#ifndef CTKEALOGTRACKER_P_H
#define CTKEALOGTRACKER_P_H

#include <service/log/ctkLogService.h>
#include <ctkServiceTracker.h>

/**
 * This class encapsulates the ctkLogService
 * and handles all issues such as the service coming and going.
 */
class ctkEALogTracker : public ctkServiceTracker<ctkLogService*>, public ctkLogService
{

public:

  /**
   * Create new ctkEALogTracker.
   *
   * @param context ctkPluginContext of parent plugin.
   * @param out Default QIODevice to use if ctkLogService is unavailable.
   */
  ctkEALogTracker(ctkPluginContext* context, QIODevice* out);

  /*
   * ----------------------------------------------------------------------
   *      LogService Interface implementation
   * ----------------------------------------------------------------------
   */

  void log(int level, const QString& message, const std::exception* exception = 0,
                   const char* file = 0, const char* function = 0, int line = -1);

  void log(const ctkServiceReference& sr, int level, const QString& message,
                   const std::exception* exception = 0,
                   const char* file = 0, const char* function = 0, int line = -1);

  int getLogLevel() const;

protected:

  ctkLogService* addingService(const ctkServiceReference& reference);

  void removedService(const ctkServiceReference& reference, ctkLogService* service);

private:

  void logToAll(const ctkServiceReference& reference, int level, const QString& message,
                const std::exception* exception, const char* file, const char* function, int line);

  /**
   * The ctkLogService is not available so we write the message to a QTextStream.
   *
   * @param reference ctkServiceReference associated with message or a default constructed one if none.
   * @param level Logging level.
   * @param message Log message.
   * @param exc Log exception or null if none.
   * @param file The file name where the log message came from.
   * @param function The function where the log message came from.
   * @param The line number where the log message came from.
   */
  void noLogService(const ctkServiceReference& reference, int level, const QString& message,
                    const std::exception* exc, const char* file, const char* function, int line);

  /** Output device to use if ctkLogService is unavailable */
  QTextStream out;

  mutable QMutex mutex;

  int logLevel;
};

#endif // CTKEALOGTRACKER_P_H
