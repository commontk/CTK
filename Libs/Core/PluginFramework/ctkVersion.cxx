#include "ctkVersion.h"

#include <QStringListIterator>

namespace ctk {

  const QString Version::SEPARATOR = ".";
  const QRegExp Version::RegExp = QRegExp("[a-zA-Z0-9_\\-]*");
  const Version Version::emptyVersion = Version(0, 0, 0);


  void Version::validate()
  {
    valid = RegExp.exactMatch(qualifier);
  }

  Version::Version(unsigned int major, unsigned int minor, unsigned int micro)
    : valid(true), major(major), minor(minor), micro(micro), qualifier("")
  {

  }

  Version::Version(unsigned int major, unsigned int minor, unsigned int micro, const QString& qualifier)
     : valid(false), major(major), minor(minor), micro(micro), qualifier(qualifier)
  {
    this->validate();
  }

  Version::Version(const QString& version)
    : valid(false), major(0), minor(0), micro(0), qualifier("")
  {
    unsigned int maj = 0;
    unsigned int min = 0;
    unsigned int mic = 0;
    QString qual("");

    QStringList st = version.split(SEPARATOR);

    if (st.empty()) return;

    QStringListIterator i(st);

    bool ok = true;
    maj = i.next().toUInt(&ok);

    if (i.hasNext())
    {
      min = i.next().toUInt(&ok);
      if (i.hasNext())
      {
        mic = i.next().toUInt(&ok);
        if (i.hasNext())
        {
          qual = i.next();
          if (i.hasNext())
          {
             ok = false;
          }
        }
      }
    }

    if (!ok) return;

    major = maj;
    minor = min;
    micro = mic;
    qualifier = qual;
    this->validate();
  }

  Version::Version(const Version& version)
  : valid(version.valid), major(version.major), minor(version.minor),
    micro(version.micro), qualifier(version.qualifier)
  {

  }

  Version Version::parseVersion(const QString& version)
  {
    if (version.isEmpty())
    {
      return emptyVersion;
    }

    QString version2 = version.trimmed();
    if (version2.isEmpty())
    {
      return emptyVersion;
    }

    return Version(version2);
  }

  unsigned int Version::getMajor() const
  {
    return major;
  }

  unsigned int Version::getMinor() const
  {
    return minor;
  }

  unsigned int Version::getMicro() const
  {
    return micro;
  }

  QString Version::getQualifier() const
  {
    return qualifier;
  }

  QString Version::toString() const
  {
    QString result;
    result += QString::number(major) + SEPARATOR + QString::number(minor) + SEPARATOR + QString::number(micro);
    if (!qualifier.isEmpty())
    {
      result += SEPARATOR + qualifier;
    }
    return result;
  }

  bool Version::operator==(const Version& other) const
  {
    if (&other == this)
    { // quicktest
      return true;
    }

    return (major == other.major) && (minor == other.minor) && (micro
        == other.micro) && qualifier == other.qualifier;
  }

  int Version::compare(const Version& other) const
  {
    if (&other == this)
    { // quicktest
      return 0;
    }

    if (major < other.major)
    {
      return -1;
    }

    if (major == other.major)
    {

      if (minor < other.minor)
      {
        return -1;
      }

      if (minor == other.minor)
      {

        if (micro < other.micro)
        {
          return -1;
        }

        if (micro == other.micro)
        {
          return qualifier.compare(other.qualifier);
        }
      }
    }
    return 1;
  }

}
