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


#ifndef CTKMTPROVIDERTRACKER_P_H
#define CTKMTPROVIDERTRACKER_P_H

#include <service/metatype/ctkMetaTypeInformation.h>

#include <ctkServiceTracker.h>

class ctkMTProviderTracker : public ctkMetaTypeInformation
{

private:

  const QSharedPointer<ctkPlugin> _plugin;
  ctkLogService* const log;
  ctkServiceTracker<>* _tracker;

public:

  // this is a simple class just used to temporarily store information about a provider
  // it is public because qHash() needs access to it
  class MetaTypeProviderWrapper;

  /**
   * Constructs a ctkMTProviderTracker which tracks all ctkMetaTypeProviders
   * registered by the specified plugin.
   * @param context The ctkPluginContext of the ctkMetaTypeService implementation
   * @param plugin The plugin to track all ctkMetaTypeProviders for.
   * @param log The <code>ctkLogService</code> to use for logging messages.
   */
  ctkMTProviderTracker(const QSharedPointer<ctkPlugin>& plugin, ctkLogService* log, ctkServiceTracker<>* tracker);

  QStringList getPids() const;
  QStringList getFactoryPids() const;
  QSharedPointer<ctkPlugin> getPlugin() const;
  ctkObjectClassDefinitionPtr getObjectClassDefinition(const QString& id, const QLocale& locale);
  QList<QLocale> getLocales() const;

private:

  QStringList getPids(bool factory) const;

  QSet<MetaTypeProviderWrapper> getMetaTypeProviders() const;

  void addMetaTypeProviderWrappers(const QString& servicePropertyName,
                                   const ctkServiceReference& serviceReference,
                                   ctkMetaTypeProvider* service, bool factory,
                                   QSet<MetaTypeProviderWrapper>& wrappers) const;

  QStringList getStringProperty(const QString& name, const QVariant& value) const;

};

#endif // CTKMTPROVIDERTRACKER_P_H
