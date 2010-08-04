#ifndef CTKCPLUGIN_H
#define CTKCPLUGIN_H

#include <ctkPluginActivator.h>

class ctkCPlugin : public QObject,
                   public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

};


#endif // CTKCPLUGIN_H
