/*
 * ctkPluginContext.h
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#ifndef CTKPLUGINCONTEXT_H_
#define CTKPLUGINCONTEXT_H_

#include <QHash>
#include <QString>
#include <QVariant>

#include "CTKCoreExport.h"


namespace ctk {

  class ServiceRegistration;
  class ServiceReference;
  class PluginContextPrivate;

  class CTK_CORE_EXPORT PluginContext
  {

	  Q_DECLARE_PRIVATE(PluginContext)

  public:

    typedef QHash<QString, QVariant> ServiceProperties;

    ~PluginContext();

    ServiceRegistration registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties = ServiceProperties());

    QList<ServiceReference> getServiceReferences(const QString& clazz, const QString& filter = QString());

    ServiceReference getServiceReference(const QString& clazz);

    QObject* getService(const ServiceReference& reference);

  protected:

    PluginContext();

    PluginContextPrivate * const d_ptr;
  };

}

#endif /* CTKPLUGINCONTEXT_H_ */
