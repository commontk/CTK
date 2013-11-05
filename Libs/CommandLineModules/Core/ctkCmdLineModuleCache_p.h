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

/**
 * \class ctkCmdLineModuleCache
 * \brief Private non-exported class to contain a cache of
 * XML descriptions and time-stamps.
 *
 * The intention is that this Cache is an in-memory representation
 * of a file-system directory containing XML files and a corresponding
 * timestamp. Hence these should always be in synch.
 *
 * \ingroup CommandLineModulesCore_API
 */
class ctkCmdLineModuleCache
{

public:

  ctkCmdLineModuleCache(const QString& cacheDir);
  ~ctkCmdLineModuleCache();

  /**
   * @brief Returns the directory containing the cached information.
   * @return a directory path
   */
  QString cacheDir() const;

  /**
   * @brief Returns the cached XML associated with a module.
   * @param moduleLocation QUrl representing the location,
   * for example a file path for a local process.
   * @return QByteArray the XML
   */
  QByteArray rawXmlDescription(const QUrl& moduleLocation) const;

  /**
   * @brief Returns the time stamp associated with a module.
   * @param moduleLocation QUrl representing the location,
   * for example a file path for a local process.
   * @return time since epoch
   */
  qint64 timeStamp(const QUrl& moduleLocation) const;

  /**
   * @brief Adds a modules XML and timestamp to the cache.
   * @param moduleLocation QUrl representing the location,
   * for example a file path for a local process.
   * @param timestamp the time
   * @param xmlDescription the XML
   */
  void cacheXmlDescription(const QUrl& moduleLocation, qint64 timestamp, const QByteArray& xmlDescription);

  /**
   * @brief Removes an entry from the cache.
   * @param moduleLocation QUrl representing the location,
   * for example a file path for a local process.
   */
  void removeCacheEntry(const QUrl& moduleLocation);

  /**
   * @brief Clears all entries from the XML/timestamp cache.
   */
  void clearCache();

private:

  QScopedPointer<ctkCmdLineModuleCachePrivate> d;
};

#endif // CTKCMDLINEMODULECACHE_H
