#ifndef CTKAPLUGIN_H
#define CTKAPLUGIN_H

#include <ctkPluginActivator.h>

class ctkAPlugin : public QObject,
                   public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

};


#endif // CTKAPLUGIN_H
