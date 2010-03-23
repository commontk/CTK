/*
 * ctkPluginContext.h
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#ifndef CTKPLUGINCONTEXT_H_
#define CTKPLUGINCONTEXT_H_

#include <QSharedPointer>
#include <QServiceInterfaceDescriptor>

#include "CTKCoreExport.h"

using namespace QtMobility;

namespace ctk {

  class PluginContextPrivate;

  class CTK_CORE_EXPORT PluginContext
  {

	  Q_DECLARE_PRIVATE(PluginContext)

  public:

    //TODO use a macro
    typedef QSharedPointer<PluginContext> Pointer;

    PluginContext();
    virtual ~PluginContext();

    QServiceInterfaceDescriptor getServiceDescriptor(const QString& interfaceName) const;

  protected:

    PluginContextPrivate * const d_ptr;
  };

}

#endif /* CTKPLUGINCONTEXT_H_ */
