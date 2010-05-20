#ifndef CTKEVENTBUSPLUGIN_H
#define CTKEVENTBUSPLUGIN_H

#include <ctkPluginActivator.h>

#include <QServicePluginInterface>


class ctkEventBusPlugin : public QObject,
                   public ctkPluginActivator,
                   public QtMobility::QServicePluginInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator QtMobility::QServicePluginInterface)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  QObject* createInstance(const QtMobility::QServiceInterfaceDescriptor& descriptor,
                          QtMobility::QServiceContext* context,
                          QtMobility::QAbstractSecuritySession* session);

};


#endif // CTKEVENTBUSPLUGIN_H
