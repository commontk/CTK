#include "ctkPluginRepository_p.h"

namespace ctk {

  const QList<Plugin*>& PluginRepository::getPlugins() const
  {
    return pluginsByInstallOrder;
  }

  Plugin* PluginRepository::getPlugin(long pluginId) const
  {
    return pluginsById[pluginId];
  }

  QList<Plugin*> PluginRepository::getPlugins(const QString& symbolicName) const
  {
    return pluginsBySymbolicName[symbolicName];
  }

  Plugin* PluginRepository::getPlugin(const QString& symbolicName, const Version& version)
  {
    const QList<Plugin*>& plugins = getPlugins(symbolicName);
    if (!plugins.isEmpty())
    {
      QListIterator<Plugin*> i(plugins);
      while (i.hasNext())
      {
        Plugin* plugin = i.next();
        if (plugin->getVersion() == version)
        {
          return plugin;
        }
      }
    }
    return 0;
  }

  void PluginRepository::add(Plugin* plugin)
  {
    pluginsByInstallOrder.push_back(plugin);
    pluginsById.insert(plugin->getPluginId(), plugin);
    addSymbolicName(plugin);
  }

  bool PluginRepository::remove(Plugin* plugin)
  {
    // remove by plugin id
    int count = pluginsById.remove(plugin->getPluginId());
    if (count == 0) return false;

    // remove by install order
    pluginsByInstallOrder.removeAll(plugin);
    // remove by symbolic name
    QString symbolicName = plugin->getSymbolicName();
    if (symbolicName.isEmpty()) return true;
    removeSymbolicName(symbolicName, plugin);
    return true;
  }

  void PluginRepository::update(const QString& oldSymbolicName, Plugin* plugin)
  {
    if (!oldSymbolicName.isEmpty())
    {
      if (oldSymbolicName != plugin->getSymbolicName())
      {
        removeSymbolicName(oldSymbolicName, plugin);
        addSymbolicName(plugin);
      }
    }
    else
    {
      addSymbolicName(plugin);
    }
  }

  void PluginRepository::removeAllPlugins() {
    pluginsByInstallOrder.clear();
    pluginsById.clear();
    pluginsBySymbolicName.clear();
  }

  void PluginRepository::addSymbolicName(Plugin* plugin)
  {
    QString symbolicName = plugin->getSymbolicName();
    if (symbolicName.isNull()) return;
    QList<Plugin*>& plugins = pluginsBySymbolicName[symbolicName];
    if (plugins.empty())
    {
      plugins.push_back(plugin);
      return;
    }

    QList<Plugin*> list;
    const Version& newVersion = plugin->getVersion();
    bool added = false;
    QListIterator<Plugin*> i(plugins);
    while (i.hasNext())
    {
      Plugin* oldPlugin = i.next();
      if (!added && newVersion.compare(oldPlugin->getVersion()) >= 0)
      {
        added = true;
        list.push_back(plugin);
      }
      list.push_back(oldPlugin);
    }
    if (!added)
    {
      list.push_back(plugin);
    }

    pluginsBySymbolicName.insert(symbolicName, list);
  }

  void PluginRepository::removeSymbolicName(const QString& symbolicName, Plugin* plugin)
  {
    QList<Plugin*>& plugins = pluginsBySymbolicName[symbolicName];
    if (plugins.isEmpty()) return;

    // found some plugins with the global name.
    // remove all references to the specified plugin.
    QMutableListIterator<Plugin*> i(plugins);
    while(i.hasNext())
    {
      if (i.next() == plugin)
        i.remove();
    }

    if (plugins.isEmpty())
    {
      pluginsBySymbolicName.remove(symbolicName);
    }
  }

}
