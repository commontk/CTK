#ifndef CTKEVENTBUSPLUGIN_H
#define CTKEVENTBUSPLUGIN_H

#include <PluginFramework/ctkPluginActivator.h>

#include <QServicePluginInterface>

using namespace QtMobility;

namespace ctk {

  class EventBusPlugin : public QObject,
                     public PluginActivator,
                     public QServicePluginInterface
  {
    Q_OBJECT
    Q_INTERFACES(ctk::PluginActivator QtMobility::QServicePluginInterface)

  public:

    void start(PluginContext* context);
    void stop(PluginContext* context);

    QObject* createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session);

  };

}

#endif // CTKEVENTBUSPLUGIN_H
