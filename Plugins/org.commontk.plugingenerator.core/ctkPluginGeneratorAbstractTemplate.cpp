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


#include "ctkPluginGeneratorAbstractTemplate.h"

#include "ctkPluginGeneratorCodeModel.h"
#include "ctkPluginGeneratorConstants.h"

#include <QHash>
#include <QFile>

class ctkPluginGeneratorAbstractTemplatePrivate
{
public:

  ctkPluginGeneratorAbstractTemplatePrivate()
    : codeModel(0)
  {}

  ctkPluginGeneratorCodeModel* codeModel;

  QString filename;
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

void ctkPluginGeneratorAbstractTemplate::setCodeModel(ctkPluginGeneratorCodeModel *codeModel)
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  d->codeModel = codeModel;
}

void ctkPluginGeneratorAbstractTemplate::setFilename(const QString& filename)
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  d->filename = filename;
}

QString ctkPluginGeneratorAbstractTemplate::getFilename() const
{
  Q_D(const ctkPluginGeneratorAbstractTemplate);

  QString filename = this->objectName();
  if(!d->filename.isEmpty())
  {
    filename = d->filename;
  }

  return filename;
}

void ctkPluginGeneratorAbstractTemplate::reset()
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  d->contentMap.clear();
}

void ctkPluginGeneratorAbstractTemplate::addContent(const QString &marker, const QString &content, Position pos)
{
  Q_D(ctkPluginGeneratorAbstractTemplate);
  switch (pos)
  {
  case PREPEND:
    {
      d->contentMap[marker].prepend(content);
      break;
    }
  case APPEND:
    {
      d->contentMap[marker].append(content);
      break;
    }
  case REPLACE:
    {
      QStringList& v = d->contentMap[marker];
      v.clear();
      v.append(content);
      break;
    }
  }
}

QStringList ctkPluginGeneratorAbstractTemplate::getContent(const QString &marker) const
{
  Q_D(const ctkPluginGeneratorAbstractTemplate);
  if (d->contentMap.contains(marker))
  {
    return d->contentMap[marker];
  }

  QString globalDefault = d->codeModel->getContent(marker);
  if (!globalDefault.isEmpty())
  {
    return QStringList(globalDefault);
  }

  return QStringList();
}

void ctkPluginGeneratorAbstractTemplate::create(const QString& location)
{
  QString filename = getFilename();

  const QString path = location + "/" + filename;
  QFile file(path);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  file.write(this->generateContent().toLatin1());
  file.close();
}

QStringList ctkPluginGeneratorAbstractTemplate::getMarkers() const
{
  return ctkPluginGeneratorConstants::getGlobalMarkers();
}

QString ctkPluginGeneratorAbstractTemplate::getSymbolicName(bool withPeriods) const
{
  Q_D(const ctkPluginGeneratorAbstractTemplate);
  return d->codeModel->getSymbolicName(withPeriods);
}
