/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkBasicLocation_p.h"

#include "ctkPluginFrameworkProperties_p.h"

#include <ctkException.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#define HAVE_QT_QLOCKFILE
#include <QLockFile>
#endif

#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>

static const QString PROP_OSGI_LOCKING = "blueberry.locking";
static const QString DEFAULT_LOCK_FILENAME = ".metadata/.lock";


//----------------------------------------------------------------------------
struct ctkBasicLocation::Locker
{
  virtual ~Locker() {}
  virtual bool lock() = 0;
  virtual bool isLocked() const = 0;
  virtual void release() = 0;
};

//----------------------------------------------------------------------------
struct ctkBasicLocation::MockLocker : public ctkBasicLocation::Locker
{
  bool lock() { return true; }
  bool isLocked() const { return false; }
  void release() {}
};

#ifdef HAVE_QT_QLOCKFILE

//----------------------------------------------------------------------------
class ctkBasicLocation::FileLocker : public ctkBasicLocation::Locker
{
  QLockFile m_locker;
  QString m_lockFile;

public:

  FileLocker(const QString& lockFile)
    : m_locker(lockFile)
    , m_lockFile(lockFile)
  {
    m_locker.setStaleLockTime(0);
  }

  bool lock()
  {
    bool result = m_locker.tryLock(100);
    if(!result)
    {
      switch (m_locker.error())
      {
      case QLockFile::PermissionError:
      {
        qDebug() << "PermissionError: Could not create lock file" << m_lockFile;
        break;
      }
      case QLockFile::LockFailedError:
      {
        qint64 pid = 0;
        QString hostname;
        QString appname;
        if (m_locker.getLockInfo(&pid, &hostname, &appname))
        {
          qDebug() << "The lock file" << m_lockFile << "on host" << hostname
                   << "is already locked by process" << pid;
        }
        else
        {
          // lock file was probably just deleted, try again
          result = m_locker.tryLock(10);
        }
        break;
      }
      case QLockFile::UnknownError:
      {
        qDebug() << "Could not create or lock file" << m_lockFile << "(unknown error, disk full?)";
        break;
      }
      case QLockFile::NoError: break;
      }
    }
    return result;
  }

  bool isLocked() const
  {
    return m_locker.isLocked();
  }

  void release()
  {
    m_locker.unlock();
  }
};

#else

//----------------------------------------------------------------------------
class ctkBasicLocation::FileLocker : public ctkBasicLocation::MockLocker
{
public:
  FileLocker(const QString&) {}
};

#endif

//----------------------------------------------------------------------------
ctkBasicLocation::ctkBasicLocation(const QString& property, const QUrl& defaultValue, bool isReadOnly, const QString& dataAreaPrefix)
  : m_isReadOnly(isReadOnly)
  , m_parent(NULL)
  , m_defaultValue(defaultValue)
  , m_property(property)
  , m_dataAreaPrefix(dataAreaPrefix)
  , m_locker(NULL)
  , m_sync()

{
}

//----------------------------------------------------------------------------
ctkBasicLocation::~ctkBasicLocation()
{
  QWriteLocker l(&this->m_sync);
  delete this->m_locker;
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::allowsDefault() const
{
  QReadLocker l(&this->m_sync);
  return !this->m_defaultValue.isEmpty();
}

//----------------------------------------------------------------------------
QUrl ctkBasicLocation::getDefault() const
{
  QReadLocker l(&this->m_sync);
  return this->m_defaultValue;
}

//----------------------------------------------------------------------------
ctkLocation* ctkBasicLocation::getParentLocation() const
{
  QReadLocker l(&this->m_sync);
  return this->m_parent.data();
}

//----------------------------------------------------------------------------
QUrl ctkBasicLocation::getUrl() const
{
  QWriteLocker l(&this->m_sync);
  if (this->m_location.isEmpty() && !this->m_defaultValue.isEmpty())
  {
    const_cast<ctkBasicLocation*>(this)->set_unlocked(this->m_defaultValue, false);
  }
  return this->m_location;
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::isSet_unlocked() const
{
  return !this->m_location.isEmpty();
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::isSet() const
{
  QReadLocker l(&this->m_sync);
  return this->isSet_unlocked();
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::isReadOnly() const
{
  QReadLocker l(&this->m_sync);
  return this->m_isReadOnly;
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::set_unlocked(const QUrl& value, bool lock)
{
  return this->set_unlocked(value, lock, QString());
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::set(const QUrl& value, bool lock)
{
  QWriteLocker l(&this->m_sync);
  return this->set_unlocked(value, lock, QString());
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::set_unlocked(const QUrl& value_, bool lock, const QString& lockFilePath)
{
  if (!this->m_location.isEmpty())
  {
    throw ctkIllegalStateException("Cannot change the location once it is set.");
  }

  QUrl value = value_;
  QFileInfo file;
  if (value.scheme().compare("file", Qt::CaseInsensitive) == 0)
  {
    QString basePath = QFileInfo(value.toLocalFile()).absolutePath();
    if (!basePath.isEmpty())
    {
      value = QUrl::fromLocalFile(basePath);
    }

    if (!lockFilePath.isEmpty())
    {
      QFileInfo givenLockFile(lockFilePath);
      if (givenLockFile.isAbsolute())
      {
        file = givenLockFile;
      }
      else
      {
        file = QFileInfo(QDir(value.toLocalFile()), lockFilePath);
      }
    }
    else
    {
      file = QFileInfo(QDir(value.toLocalFile()), DEFAULT_LOCK_FILENAME);
    }
  }
  lock = lock && !this->m_isReadOnly;
  if (lock)
  {
    if (!this->lock_unlocked(file, value))
      return false;
  }
  this->m_lockFile = file;
  this->m_location = value;
  if (!this->m_property.isEmpty())
  {
    ctkPluginFrameworkProperties::setProperty(this->m_property, this->m_location.toString());
  }
  return lock;
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::set(const QUrl& value, bool lock, const QString& lockFilePath)
{
  QWriteLocker l(&this->m_sync);
  return this->set_unlocked(value, lock, lockFilePath);
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::lock()
{
  QWriteLocker l(&this->m_sync);
  if (!isSet_unlocked())
  {
    throw ctkRuntimeException("The location has not been set.");
  }
  return this->lock_unlocked(this->m_lockFile, this->m_location);
}

//----------------------------------------------------------------------------
void ctkBasicLocation::release()
{
  QWriteLocker l(&this->m_sync);
  if (this->m_locker)
  {
    this->m_locker->release();
  }
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::isLocked() const
{
  QReadLocker l(&this->m_sync);
  if (!isSet_unlocked()) return false;
  return this->isLocked_unlocked(this->m_lockFile);
}

//----------------------------------------------------------------------------
ctkLocation* ctkBasicLocation::createLocation(ctkLocation* parent, const QUrl& defaultValue, bool readOnly)
{
  QWriteLocker l(&this->m_sync);
  ctkBasicLocation* result = new ctkBasicLocation(QString(), defaultValue, readOnly, this->m_dataAreaPrefix);
  result->m_parent.reset(parent);
  return result;
}

//----------------------------------------------------------------------------
QUrl ctkBasicLocation::getDataArea(const QString& filename_) const
{
  QUrl base = getUrl();
  if (base.isEmpty())
  {
    throw ctkRuntimeException("The location has not been set.");
  }
  QString prefix = base.toLocalFile();
  if (!prefix.isEmpty() && prefix.at(prefix.size() - 1) != '/')
  {
    prefix += '/';
  }
  QString filename = filename_;
  filename.replace('\\', '/');
  if (!filename.isEmpty() && filename.at(0) == '/')
  {
    filename = filename.mid(1);
  }

  QReadLocker l(&this->m_sync);
  return QUrl(prefix + this->m_dataAreaPrefix + filename);
}

//----------------------------------------------------------------------------
void ctkBasicLocation::setParent(ctkLocation* parent)
{
  QWriteLocker l(&this->m_sync);
  this->m_parent.reset(parent);
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::lock_unlocked(const QFileInfo& lock, const QUrl& locationValue)
{
  if (this->m_isReadOnly)
  {
    throw ctkRuntimeException(QString("The folder \"%1\" is read-only.").arg(lock.absoluteFilePath()));
  }

  if (lock.fileName().isEmpty())
  {
    if (locationValue.scheme().compare("file", Qt::CaseInsensitive) != 0)
    {
      throw ctkRuntimeException(QString("Unable to lock the location. The set location is not a file URL: ") + locationValue.toString());
    }
    throw ctkIllegalStateException("The lock file has not been set"); // this is really unexpected
  }

  if (this->isLocked())
  {
    return false;
  }

  QDir parentDir = lock.dir();
  if (!parentDir.exists())
  {
    if (!parentDir.mkpath(lock.canonicalPath()))
    {
        throw ctkRuntimeException(QString("The folder \"%1\" is read-only.").arg(parentDir.absolutePath()));
    }
  }

  setLocker_unlocked(lock);
  if (this->m_locker == NULL)
  {
    return true;
  }
  bool locked = this->m_locker->lock();
  if (!locked) delete this->m_locker;
  return locked;
}

//----------------------------------------------------------------------------
void ctkBasicLocation::setLocker_unlocked(const QFileInfo& lock)
{
  if (this->m_locker != NULL) return;

  QString lockMode = ctkPluginFrameworkProperties::getProperty(PROP_OSGI_LOCKING).toString();

  this->m_locker = this->createLocker_unlocked(lock, lockMode);
}

//----------------------------------------------------------------------------
ctkBasicLocation::Locker* ctkBasicLocation::createLocker_unlocked(const QFileInfo& lock, const QString& lockMode_)
{
  QString lockMode = lockMode_;
  if (lockMode.isEmpty())
  {
    lockMode = ctkPluginFrameworkProperties::getProperty(PROP_OSGI_LOCKING).toString();
  }

  if (lockMode == "none")
  {
    return new MockLocker();
  }

  if (lockMode == "qt.lockfile")
  {
    return new FileLocker(lock.canonicalFilePath());
  }

  // Backup case if an invalid value has been specified
  return new FileLocker(lock.canonicalFilePath());
}

//----------------------------------------------------------------------------
bool ctkBasicLocation::isLocked_unlocked(const QFileInfo& lock) const
{
  if (lock.fileName().isEmpty() || this->m_isReadOnly)
  {
    return true;
  }
  if (!lock.exists())
  {
    return false;
  }
  const_cast<ctkBasicLocation*>(this)->setLocker_unlocked(lock);
  return this->m_locker->isLocked();
}
