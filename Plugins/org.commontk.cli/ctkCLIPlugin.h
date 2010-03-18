/*
 * ctkCLIPlugin.h
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#ifndef CTKCLIPLUGIN_H_
#define CTKCLIPLUGIN_H_

#include <PluginFramework/ctkPluginActivator.h>

#include <QServicePluginInterface>

using namespace QtMobility;

namespace ctk {

  class CLIPlugin : public QObject,
                    public PluginActivator,
                    public QServicePluginInterface
  {
    Q_OBJECT
    Q_INTERFACES(ctk::PluginActivator QtMobility::QServicePluginInterface)

  public:

    void start(PluginContext::Pointer context);
    void stop(PluginContext::Pointer context);

    QObject* createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session);

  };

}

#endif /* CTKCLIPLUGIN_H_ */
