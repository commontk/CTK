#ifndef CTKPLUGINREPOSITORY_P_H
#define CTKPLUGINREPOSITORY_P_H

#include "ctkPlugin.h"

#include "ctkVersion.h"

#include <QList>
#include <QHash>


namespace ctk {

  class PluginRepository {

  private:

    QList<Plugin*> pluginsByInstallOrder;
    QHash<long, Plugin*> pluginsById;
    QHash<QString, QList<Plugin*> > pluginsBySymbolicName;

  public:

    const QList<Plugin*>& getPlugins() const;

    Plugin* getPlugin(long pluginId) const;

    QList<Plugin*> getPlugins(const QString& symbolicName) const;

    Plugin* getPlugin(const QString& symbolicName, const Version& version);

    void add(Plugin* plugin);

    bool remove(Plugin* plugin);

    void update(const QString& oldSymbolicName, Plugin* plugin);

    void removeAllPlugins();

  private:

    void addSymbolicName(Plugin* plugin);

    void removeSymbolicName(const QString& symbolicName, Plugin* plugin);

  };

}

#endif // CTKPLUGINREPOSITORY_P_H
