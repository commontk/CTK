#ifndef CTKPLUGINPRIVATE_P_H
#define CTKPLUGINPRIVATE_P_H

namespace ctk {

  class PluginPrivate {

  public:

    virtual ~PluginPrivate();

    void load();

    QHash<QString, QString> getHeaders(const QString& )

  };

}

#endif // CTKPLUGINPRIVATE_P_H
