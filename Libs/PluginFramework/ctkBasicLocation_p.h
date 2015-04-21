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

#ifndef CTKBASICLOCATION_H
#define CTKBASICLOCATION_H

#include <service/datalocation/ctkLocation.h>

#include <QUrl>
#include <QString>
#include <QFileInfo>
#include <QReadWriteLock>

class ctkBasicLocation : public QObject, public ctkLocation
{
  Q_OBJECT
  Q_INTERFACES(ctkLocation)

public:

  ctkBasicLocation(const QString& property, const QUrl& defaultValue,
           bool isReadOnly, const QString& dataAreaPrefix);

  ~ctkBasicLocation();

  bool allowsDefault() const;

  QUrl getDefault() const;

  ctkLocation* getParentLocation() const;

  QUrl getUrl() const;

  bool isSet() const;

  bool isReadOnly() const;

  bool set(const QUrl& value, bool lock);

  bool set(const QUrl& value, bool lock, const QString& lockFilePath);

  bool lock();

  void release();

  bool isLocked() const;

  ctkLocation* createLocation(ctkLocation* parent, const QUrl& defaultValue, bool readonly);

  QUrl getDataArea(const QString& path) const;

  void setParent(ctkLocation* parent);

  struct Locker;

private:

  struct MockLocker;
  class FileLocker;

  bool m_isReadOnly;
  QUrl m_location;
  QScopedPointer<ctkLocation> m_parent;
  QUrl m_defaultValue;
  QString m_property;
  QString m_dataAreaPrefix;

  // locking related fields
  QFileInfo m_lockFile;
  Locker* m_locker;

  mutable QReadWriteLock m_sync;

  bool isSet_unlocked() const;

  bool set_unlocked(const QUrl& value, bool lock);
  bool set_unlocked(const QUrl& value, bool lock, const QString& lockFilePath);

  bool lock_unlocked(const QFileInfo& lock, const QUrl& locationValue);
  void setLocker_unlocked(const QFileInfo& lock);
  Locker* createLocker_unlocked(const QFileInfo& lock, const QString& lockMode);
  bool isLocked_unlocked(const QFileInfo& lock) const;

};

#endif // CTKBASICLOCATION_H
