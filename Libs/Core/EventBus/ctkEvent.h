#ifndef CTKEVENT_H
#define CTKEVENT_H

#include "CTKCoreExport.h"

#include <QMap>
#include <QVariant>
#include <QStringList>

#include <PluginFramework/ctkLDAPSearchFilter.h>

namespace ctk {

  class EventPrivate;

  class CTK_CORE_EXPORT Event {

  public:

    typedef QMap<QString, QVariant> Properties;

    //TODO: what are we doing about malformed topic strings? Use exceptions in CTK?
    Event(const QString& topic, const LDAPSearchFilter::Dictionary& properties = Properties());
    Event(const Event& event);
    ~Event();

    bool operator==(const Event& other) const;

    const QVariant& property(const QString& name) const;
    QStringList propertyNames() const;

    const QString& topic() const;

    bool matches(const LDAPSearchFilter& filter) const;

  protected:

    EventPrivate * const d;
  };

}

#endif // CTKEVENT_H
