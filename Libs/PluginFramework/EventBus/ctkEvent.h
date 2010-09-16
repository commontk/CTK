#ifndef CTKEVENT_H
#define CTKEVENT_H

#include "CTKPluginFrameworkExport.h"

#include <QMap>
#include <QVariant>
#include <QStringList>

#include <ctkLDAPSearchFilter.h>


  class ctkEventPrivate;

  class CTK_PLUGINFW_EXPORT ctkEvent {

  public:

    ctkEvent(const QString& topic, const ctkDictionary& properties = ctkDictionary());
    ctkEvent(const ctkEvent& event);
    ~ctkEvent();

    bool operator==(const ctkEvent& other) const;

    QVariant property(const QString& name) const;
    QStringList propertyNames() const;

    const QString& topic() const;

    bool matches(const ctkLDAPSearchFilter& filter) const;

  protected:

    ctkEventPrivate * const d;
  };


#endif // CTKEVENT_H
