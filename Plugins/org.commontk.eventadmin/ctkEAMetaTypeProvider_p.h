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

#ifndef CTKEAMETATYPEPROVIDER_P_H
#define CTKEAMETATYPEPROVIDER_P_H

#include <QObject>

#include <service/metatype/ctkMetaTypeProvider.h>
#include <service/cm/ctkManagedService.h>

/**
 * The optional meta type provider for the event admin config.
 */
class ctkEAMetaTypeProvider : public QObject, public ctkMetaTypeProvider, public ctkManagedService
{
  Q_OBJECT
  Q_INTERFACES(ctkMetaTypeProvider ctkManagedService)

private:

  const int m_cacheSize;
  const int m_threadPoolSize;
  const int m_timeout;
  const bool m_requireTopic;
  const QStringList m_ignoreTimeout;

  ctkManagedService* const m_delegatee;

  ctkObjectClassDefinitionPtr ocd;

  class ObjectClassDefinitionImpl;
  class AttributeDefinitionImpl;

public:

  ctkEAMetaTypeProvider(ctkManagedService* delegatee, int cacheSize,
                        int threadPoolSize, int timeout, bool requireTopic,
                        const QStringList& ignoreTimeout);


  /**
   * @see ctkManagedService#updated(ctkDictionary)
   */
  void updated(const ctkDictionary& properties);

  /**
   * @see ctkMetaTypeProvider#getLocales()
   */
  QList<QLocale> getLocales() const;

  /**
   * @see ctkMetaTypeProvider#getObjectClassDefinition(QString, QString)
   */
  ctkObjectClassDefinitionPtr getObjectClassDefinition(const QString& id, const QLocale& locale);

};

#endif // CTKEAMETATYPEPROVIDER_P_H
