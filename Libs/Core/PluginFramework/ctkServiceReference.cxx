#include "ctkServiceReference.h"

#include <QStringList>

namespace ctk {

  QVariant ServiceReference::getProperty(const QString& key) const
  {
    return QVariant();
  }

  QStringList ServiceReference::getPropertyKeys() const
  {
    return QStringList();
  }

  Plugin* ServiceReference::getPlugin() const
  {
    return 0;
  }

  QList<Plugin*> ServiceReference::getUsingPlugins() const
  {
    return QList<Plugin*>();
  }

  bool ServiceReference::operator<(const ServiceReference& reference) const
  {
    return false;
  }

}
