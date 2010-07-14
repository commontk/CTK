/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#include "ctkPluginGeneratorAbstractTemplate.h"

#include <QHash>

class ctkPluginGeneratorAbstractTemplatePrivate
{
public:

  QHash<QString, QString> contentMap;
};

ctkPluginGeneratorAbstractTemplate::ctkPluginGeneratorAbstractTemplate(
    const QString& name, QObject* parent)
      : QObject(parent), d_ptr(new ctkPluginGeneratorAbstractTemplatePrivate)
{
  this->setObjectName(name);
}

void ctkPluginGeneratorAbstractTemplate::AddContent(const QString &marker, const QString &content)
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  d->contentMap.insert(marker, content);
}

QString ctkPluginGeneratorAbstractTemplate::GetContent(const QString &marker) const
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  if (d->contentMap.contains(marker))
  {
    return d->contentMap[marker];
  }

  return QString();
}

QStringList ctkPluginGeneratorAbstractTemplate::GetMarkers() const
{
  return QStringList();
}

