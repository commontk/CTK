/*
 * ctkCLIPlugin.h
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#ifndef CTKCLIPLUGIN_H_
#define CTKCLIPLUGIN_H_

#include <ctkPluginActivator.h>

#include <QServicePluginInterface>

class ctkCLIPlugin : public QObject,
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


#endif /* CTKCLIPLUGIN_H_ */
