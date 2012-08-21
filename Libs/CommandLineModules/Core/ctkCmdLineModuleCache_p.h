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

#ifndef CTKCMDLINEMODULECACHE_H
#define CTKCMDLINEMODULECACHE_H

#include <QScopedPointer>

struct ctkCmdLineModuleCachePrivate;

class QUrl;

class ctkCmdLineModuleCache
{

public:

  ctkCmdLineModuleCache(const QString& cacheDir);
  ~ctkCmdLineModuleCache();

  QString cacheDir() const;

  QByteArray rawXmlDescription(const QUrl& moduleLocation) const;
  qint64 timeStamp(const QUrl& moduleLocation) const;

  void cacheXmlDescription(const QUrl& moduleLocation, qint64 timestamp, const QByteArray& xmlDescription);

  void removeCacheEntry(const QUrl& moduleLocation);

private:

  QScopedPointer<ctkCmdLineModuleCachePrivate> d;
};

#endif // CTKCMDLINEMODULECACHE_H
