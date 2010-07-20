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
#include <QFile>

class ctkPluginGeneratorAbstractTemplatePrivate
{
public:

  QHash<QString, QStringList> contentMap;
};

ctkPluginGeneratorAbstractTemplate::ctkPluginGeneratorAbstractTemplate(
    const QString& name, ctkPluginGeneratorAbstractTemplate* parent)
      : QObject(parent), d_ptr(new ctkPluginGeneratorAbstractTemplatePrivate)
{
  this->setObjectName(name);
}

ctkPluginGeneratorAbstractTemplate::~ctkPluginGeneratorAbstractTemplate()
{

}

void ctkPluginGeneratorAbstractTemplate::addContent(const QString &marker, const QString &content, Position pos)
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  if (pos == START)
  {
    d->contentMap[marker].prepend(content);
  }
  else if (pos == END)
  {
    d->contentMap[marker].append(content);
  }
}

QStringList ctkPluginGeneratorAbstractTemplate::getContent(const QString &marker) const
{
  Q_D(const ctkPluginGeneratorAbstractTemplate);
  if (d->contentMap.contains(marker))
  {
    return d->contentMap[marker];
  }

  return QStringList();
}

void ctkPluginGeneratorAbstractTemplate::create(const QString& location)
{
  const QString filename = location + "/" + this->objectName();
  QFile file(filename);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  file.write(this->generateContent().toAscii());
  file.close();
}

QStringList ctkPluginGeneratorAbstractTemplate::getMarkers() const
{
  return QStringList();
}

