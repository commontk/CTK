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

#include "ctkEAMetaTypeProvider_p.h"

#include "ctkEAConfiguration_p.h"

#include <limits>


class ctkEAMetaTypeProvider::ObjectClassDefinitionImpl : public ctkObjectClassDefinition
{

private:

  QList<ctkAttributeDefinitionPtr> attrs;

public:

  ObjectClassDefinitionImpl(const QList<ctkAttributeDefinitionPtr>& attrs)
    : attrs(attrs)
  {

  }

  QString getName() const
  {
    return "CommonTK Event Admin Implementation";
  }

  QByteArray getIcon(int size) const
  {
    Q_UNUSED(size)
    return QByteArray();
  }

  QString getID() const
  {
    return ctkEAConfiguration::PID;
  }

  QString getDescription() const
  {
    return "Configuration for the CommonTK Event Admin Implementation."
        " This configuration overwrites configuration defined in framework properties of the same names.";
  }

  QList<ctkAttributeDefinitionPtr> getAttributeDefinitions(Filter filter)
  {
    return (filter == OPTIONAL) ? QList<ctkAttributeDefinitionPtr>() : attrs;
  }
};

class ctkEAMetaTypeProvider::AttributeDefinitionImpl : public ctkAttributeDefinition
{

private:

  const QString id;
  const QString name;
  const QString description;
  const int type;
  const QStringList defaultValues;
  const int cardinality;
  const QStringList optionLabels;
  const QStringList optionValues;


public:

  AttributeDefinitionImpl(const QString& id, const QString& name, const QString& description, int type,
                          const  QStringList& defaultValues, int cardinality = 0,
                          const QStringList& optionLabels = QStringList(),
                          const QStringList& optionValues = QStringList())
    : id(id), name(name), description(description), type(type),
      defaultValues(defaultValues), cardinality(cardinality),
      optionLabels(optionLabels), optionValues(optionValues)
  {

  }

  int getCardinality() const
  {
    return cardinality;
  }

  QStringList getDefaultValue() const
  {
    return defaultValues;
  }

  QString getDescription() const
  {
    return description;
  }

  QString getID() const
  {
    return id;
  }

  QString getName() const
  {
    return name;
  }

  QStringList getOptionLabels() const
  {
    return optionLabels;
  }

  QStringList getOptionValues() const
  {
    return optionValues;
  }

  int getType() const
  {
    return type;
  }

  QString validate(const QString& str) const
  {
    Q_UNUSED(str)
    return QString();
  }
};


ctkEAMetaTypeProvider::ctkEAMetaTypeProvider(ctkManagedService* delegatee, int cacheSize,
                                             int threadPoolSize, int timeout, bool requireTopic,
                                             const QStringList& ignoreTimeout)
  : m_cacheSize(cacheSize), m_threadPoolSize(threadPoolSize), m_timeout(timeout),
    m_requireTopic(requireTopic), m_ignoreTimeout(ignoreTimeout), m_delegatee(delegatee)
{
}

void ctkEAMetaTypeProvider::updated(const ctkDictionary& properties)
{
  m_delegatee->updated(properties);
}

QList<QLocale> ctkEAMetaTypeProvider::getLocales() const
{
  return QList<QLocale>();
}

ctkObjectClassDefinitionPtr ctkEAMetaTypeProvider::getObjectClassDefinition(const QString& id, const QLocale& locale)
{
  Q_UNUSED(locale)

  if (ctkEAConfiguration::PID != id)
  {
    return ctkObjectClassDefinitionPtr();
  }

  if (!ocd)
  {
    QList<ctkAttributeDefinitionPtr> adList;

    adList.push_back(ctkAttributeDefinitionPtr(
                       new AttributeDefinitionImpl(ctkEAConfiguration::PROP_CACHE_SIZE, "Cache Size",
                                                   "The size of various internal caches. The default value is 30. Increase in case "
                                                   "of a large number (more then 100) of services. A value less then 10 triggers the "
                                                   "default value.", QVariant::Int, QStringList(QString::number(m_cacheSize)))));

    adList.push_back(ctkAttributeDefinitionPtr(
                       new AttributeDefinitionImpl(ctkEAConfiguration::PROP_THREAD_POOL_SIZE, "Thread Pool Size",
                                                   "The size of the thread pool. The default value is 10. Increase in case of a large amount "
                                                   "of synchronous events where the event handler services in turn send new synchronous events in "
                                                   "the event dispatching thread or a lot of timeouts are to be expected. A value of "
                                                   "less then 2 triggers the default value. A value of 2 effectively disables thread pooling.",
                                                   QVariant::Int, QStringList(QString::number(m_threadPoolSize)))));

    adList.push_back(ctkAttributeDefinitionPtr(
                       new AttributeDefinitionImpl(ctkEAConfiguration::PROP_TIMEOUT, "Timeout",
                                                   "The black-listing timeout in milliseconds. The default value is 5000. Increase or decrease "
                                                   "at own discretion. A value of less then 100 turns timeouts off. Any other value is the time "
                                                   "in milliseconds granted to each event handler before it gets blacklisted",
                                                   QVariant::Int, QStringList(QString::number(m_timeout)))));

    adList.push_back(ctkAttributeDefinitionPtr(
                       new AttributeDefinitionImpl(ctkEAConfiguration::PROP_REQUIRE_TOPIC, "Require Topic",
                                                   "Are event handlers required to be registered with a topic? "
                                                   "This is enabled by default. The specification says that event handlers "
                                                   "must register with a list of topics they are interested in. Disabling this setting "
                                                   "will enable that handlers without a topic are receiving all events "
                                                   "(i.e., they are treated the same as with a topic=*).",
                                                   QVariant::Bool, m_requireTopic ? QStringList("true") : QStringList("false"))));

    adList.push_back(ctkAttributeDefinitionPtr(
                       new AttributeDefinitionImpl(ctkEAConfiguration::PROP_IGNORE_TIMEOUT, "Ignore Timeouts",
                                                   "Configure event handlers to be called without a timeout. If a timeout is configured by default "
                                                   "all event handlers are called using the timeout. For performance optimization it is possible to "
                                                   "configure event handlers where the timeout handling is not used - this reduces the thread usage "
                                                   "from the thread pools as the timout handling requires an additional thread to call the event "
                                                   "handler. However, the application should work without this configuration property. It is a "
                                                   "pure optimization! The value is a list of strings (separated by comma) which is assumed to define "
                                                   "exact class names.",
                                                   QVariant::String, m_ignoreTimeout, 0,
                                                   QStringList(QString::number(std::numeric_limits<int>::max())))));

    ocd = ctkObjectClassDefinitionPtr(new ObjectClassDefinitionImpl(adList));
  }

  return ocd;
}

