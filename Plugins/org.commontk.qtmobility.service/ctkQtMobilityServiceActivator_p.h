#ifndef CTKEVENTBUSPLUGIN_H
#define CTKEVENTBUSPLUGIN_H

#include <ctkPluginActivator.h>

class QObject;

class ctkLogService;
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
