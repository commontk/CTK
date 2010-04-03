#ifndef CTKSERVICEREFERENCE_H
#define CTKSERVICEREFERENCE_H

#include <QVariant>

#include "ctkPlugin.h"

namespace ctk {

  class ServiceReference {

  public:

    QVariant getProperty(const QString& key) const;

    QStringList getPropertyKeys() const;

    Plugin* getPlugin() const;

    QList<Plugin*> getUsingPlugins() const;

    bool operator<(const ServiceReference& reference) const;

  };

}

#endif // CTKSERVICEREFERENCE_H
