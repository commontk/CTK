#ifndef CTKPLUGINEXCEPTION_H
#define CTKPLUGINEXCEPTION_H

#include <stdexcept>

#include <QString>
#include <QDebug>

namespace ctk {

  class PluginException : public std::runtime_error
  {
  public:

    enum Type {
      /**
       * No exception type is unspecified.
       */
      UNSPECIFIED,
      /**
       * The operation was unsupported.
       */
      UNSUPPORTED_OPERATION,
      /**
       * The operation was invalid.
       */
      INVALID_OPERATION,
      /**
       * The bundle manifest contains errors.
       */
      MANIFEST_ERROR,
      /**
       * The bundle was not resolved.
       */
      RESOLVE_ERROR,
      /**
       * The bundle activator was in error.
       */
      ACTIVATOR_ERROR,
      /**
       * The operation failed due to insufficient permissions.
       */
      SECURITY_ERROR,
      /**
       * The operation failed to complete the requested lifecycle state change.
       */
      STATECHANGE_ERROR,
      /**
       * The install or update operation failed because another
       * already installed bundle has the same symbolic name and version.
       */
      DUPLICATE_BUNDLE_ERROR
    };

    PluginException(const QString& msg, const Type& type = UNSPECIFIED, const std::exception& cause = std::exception());
    PluginException(const QString& msg, const std::exception& cause);

    PluginException(const PluginException& o);
    PluginException& operator=(const PluginException& o);

    ~PluginException() throw() {}

    std::exception getCause() const;
    void setCause(const std::exception&) throw(std::logic_error);
    Type getType() const;

    const char* what() const throw();

  private:

    const QString msg;
    const Type type;
    std::exception cause;

  };

}

QDebug operator<<(QDebug dbg, const ctk::PluginException& exc);

#endif // CTKPLUGINEXCEPTION_H
