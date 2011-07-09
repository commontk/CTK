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

#ifndef CTKEVENTBUSPLUGIN_H
#define CTKEVENTBUSPLUGIN_H

#include <ctkPluginActivator.h>

class QObject;

struct ctkLogService;
template<class S, class T> class ctkServiceTracker;
class ctkQtMobilityServiceRuntime;

class ctkQtMobilityServiceActivator : public QObject,
                                      public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  static ctkLogService* getLogService();

  static void logError(ctkPluginContext* pc, const QString& msg, std::exception* e = 0);
  static void logWarning(ctkPluginContext* pc, const QString& msg, std::exception* e = 0);
  static void logInfo(ctkPluginContext* pc, const QString& msg, std::exception* e = 0);


private:

  static void logPluginContext(ctkPluginContext* pc, int level, const QString& msg, const std::exception* e);

  ctkQtMobilityServiceRuntime* mobsr;

  typedef ctkServiceTracker<ctkLogService*,ctkLogService*> LogTracker;

  static LogTracker* logTracker;

};


#endif // CTKEVENTBUSPLUGIN_H
