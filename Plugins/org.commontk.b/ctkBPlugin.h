#ifndef CTKBPLUGIN_H
#define CTKBPLUGIN_H

#include <ctkPluginActivator.h>

class ctkBPlugin : public QObject,
                   public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

};


#endif // CTKBPLUGIN_H
