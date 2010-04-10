#include "ctkPluginException.h"

namespace ctk {

  PluginException::PluginException(const QString& msg, const Type& type = UNSPECIFIED, const std::exception& cause = std::exception())
    : msg(msg), type(type), cause(cause)
  {

  }

  PluginException::PluginException(const QString& msg, const std::exception& cause)
    : msg(msg), type(UNSPECIFIED), cause(cause)
  {

  }

  PluginException::PluginException(const PluginException& o)
    : msg(o.msg), type(o.type), cause(o.cause)
  {

  }

  PluginException& PluginException::operator=(const PluginException& o)
  {
    msg = o.msg;
    type = o.type;
    cause = o.cause;
    return *this;
  }

  std::exception PluginException::getCause() const
  {
    return cause;
  }

  void PluginException::setCause(const std::exception& cause) throw(std::logic_error)
  {
    if (!cause.what()) throw std::logic_error("The cause for this PluginException instance is already set");

    this->cause = cause;
  }

  Type PluginException::getType() const
  {
    return type;
  }

  const char* PluginException::what() const throw()
  {
    return qPrintable(*this);
  }

}

QDebug operator<<(QDebug dbg, const PluginException& exc)
{
  dbg << "PluginException:" << msg;

  const char* causeMsg = cause.what();
  if (causeMsg) dbg << "  Caused by:" << cause.what();

  return dbg.maybeSpace();
}
