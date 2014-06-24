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

#include "ctkCmdLineModuleCache_p.h"

#include <QUrl>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>
#include <QMutex>
#include <QHash>

#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
#include "ctkCommandLineModulesCoreExport.h"
CTK_CMDLINEMODULECORE_EXPORT int qHash(const QUrl& url)
{
  return qHash(url.toString());
}
#endif

struct ctkCmdLineModuleCachePrivate
{
  QString CacheDir;

  QHash<QUrl, qint64> LocationToTimeStamp;
  QHash<QUrl, QByteArray> LocationToXmlDescription;

  QMutex Mutex;

  void LoadTimeStamps()
  {
    QDirIterator dirIter(this->CacheDir, QStringList() << "*.timestamp", QDir::Files | QDir::Readable);
    while(dirIter.hasNext())
    {
      QFile timestampFile(dirIter.next());
      timestampFile.open(QIODevice::ReadOnly);
      QUrl url = QUrl(timestampFile.readLine().trimmed().data());
      QByteArray timestamp = timestampFile.readLine();
      bool ok = false;
      qint64 ts = timestamp.toLongLong(&ok);
      if (ok && !url.isEmpty())
      {
        this->LocationToTimeStamp[url] = ts;
      }
    }
  }

  QString timeStampFileName(const QUrl& moduleLocation) const
  {
    return this->CacheDir + "/" + QString::number(qHash(moduleLocation)) + ".timestamp";
  }

  QString xmlFileName(const QUrl& moduleLocation) const
  {
    return this->CacheDir + "/" + QString::number(qHash(moduleLocation)) + ".xml";
  }
};

ctkCmdLineModuleCache::ctkCmdLineModuleCache(const QString& cacheDir)
  : d(new ctkCmdLineModuleCachePrivate)
{
  d->CacheDir = cacheDir;
  d->LoadTimeStamps();
}

ctkCmdLineModuleCache::~ctkCmdLineModuleCache()
{
}

QString ctkCmdLineModuleCache::cacheDir() const
{
  QMutexLocker lock(&d->Mutex);
  return d->CacheDir;
}

QByteArray ctkCmdLineModuleCache::rawXmlDescription(const QUrl& moduleLocation) const
{
  QMutexLocker lock(&d->Mutex);

  if (d->LocationToXmlDescription.contains(moduleLocation))
  {
    return d->LocationToXmlDescription[moduleLocation];
  }
  // lazily load the XML description from the file system
  QByteArray xml;
  QString a = moduleLocation.toString();
  QString fn = d->xmlFileName(moduleLocation);
  QFile xmlFile(d->xmlFileName(moduleLocation));
  if (xmlFile.exists() && xmlFile.open(QIODevice::ReadOnly))
  {
    xml = xmlFile.readAll();
    xmlFile.close();
  }
  d->LocationToXmlDescription[moduleLocation] = xml;
  return xml;
}

qint64 ctkCmdLineModuleCache::timeStamp(const QUrl& moduleLocation) const
{
  QMutexLocker lock(&d->Mutex);
  if (d->LocationToTimeStamp.contains(moduleLocation))
  {
    return d->LocationToTimeStamp[moduleLocation];
  }
  return -1;
}

void ctkCmdLineModuleCache::cacheXmlDescription(const QUrl& moduleLocation, qint64 timestamp, const QByteArray& xmlDescription)
{
  QFile timestampFile(d->timeStampFileName(moduleLocation));
  QFile xmlFile(d->xmlFileName(moduleLocation));
  timestampFile.remove();
  timestampFile.open(QIODevice::WriteOnly);

  QByteArray ba;
  QTextStream str(&ba);
  str << moduleLocation.toString() << '\n' << timestamp;
  str.flush();
  if (timestampFile.write(ba) == -1)
  {
    timestampFile.close();
    timestampFile.remove();
    return;
  }
  timestampFile.close();

  xmlFile.remove();
  if (!xmlDescription.isEmpty())
  {
    xmlFile.open(QIODevice::WriteOnly);
    if (xmlFile.write(xmlDescription) == -1)
    {
      timestampFile.remove();
      xmlFile.close();
      xmlFile.remove();
      return;
    }
  }

  {
    QMutexLocker lock(&d->Mutex);
    d->LocationToXmlDescription[moduleLocation] = xmlDescription;
    d->LocationToTimeStamp[moduleLocation] = timestamp;
  }
}

void ctkCmdLineModuleCache::removeCacheEntry(const QUrl& moduleLocation)
{
  {
    QMutexLocker lock(&d->Mutex);
    d->LocationToTimeStamp.remove(moduleLocation);
    d->LocationToXmlDescription.remove(moduleLocation);
  }

  QFile timestampFile(d->timeStampFileName(moduleLocation));
  if (timestampFile.exists())
  {
    timestampFile.remove();
  }
  QFile xmlFile(d->xmlFileName(moduleLocation));
  if (xmlFile.exists())
  {
    xmlFile.remove();
  }
}

void ctkCmdLineModuleCache::clearCache()
{
  foreach(const QUrl &url, d->LocationToXmlDescription.keys())
  {
    removeCacheEntry(url);
  }
}
