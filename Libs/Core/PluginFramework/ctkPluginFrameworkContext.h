#ifndef CTKPLUGINFRAMEWORKCONTEXT_H
#define CTKPLUGINFRAMEWORKCONTEXT_H

#include <QHash>
#include <QString>
#include <QVariant>

namespace ctk {

  class PluginFrameworkContextPrivate;

  class PluginFrameworkContext {

    Q_DECLARE_PRIVATE(PluginFrameworkContext)

  public:

    typedef QHash<QString, QVariant> Properties;

    PluginFrameworkContext(const Properties& initProps);
    ~PluginFrameworkContext();

  private:

    PluginFrameworkContextPrivate * const d_ptr;

  };

}

#endif // CTKPLUGINFRAMEWORKCONTEXT_H
