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


#include "ctkPluginGeneratorCodeModel.h"

#include <QDir>
#include <QHash>

class ctkPluginGeneratorCodeModelPrivate
{
public:

  QHash<QString, ctkPluginGeneratorAbstractTemplate*> rootTemplates;

};

class ctkPluginGeneratorFolderTemplate : public ctkPluginGeneratorAbstractTemplate
{
public:

  ctkPluginGeneratorFolderTemplate(const QString& name, ctkPluginGeneratorAbstractTemplate* parent)
    : ctkPluginGeneratorAbstractTemplate(name, parent)
  {}

  QString generateContent()
  {
    return "";
  }

  void create(const QString& location)
  {
    QDir dir(location);
    if (dir.mkdir(this->objectName()))
    {
      QString currLocation = location + QDir::separator() + this->objectName();
      QListIterator<QObject*> it(this->children());
      while (it.hasNext())
      {
        qobject_cast<ctkPluginGeneratorAbstractTemplate*>(it.next())->create(currLocation);
      }
    }
    else
    {
      throw std::exception("The directory " + location.append(this->objectName()).toAscii() + " could not be created");
    }
  }
};

ctkPluginGeneratorCodeModel::ctkPluginGeneratorCodeModel()
  : d_ptr(new ctkPluginGeneratorCodeModelPrivate)
{
}

ctkPluginGeneratorCodeModel::~ctkPluginGeneratorCodeModel()
{
}

void ctkPluginGeneratorCodeModel::addTemplate(ctkPluginGeneratorAbstractTemplate *templ,
                                              const QString& path)
{
  Q_D(ctkPluginGeneratorCodeModel);
  if (path.isEmpty())
  {
    d->rootTemplates.insert(templ->objectName(), templ);
  }
  else
  {
    ctkPluginGeneratorAbstractTemplate* parentTemplate = 0;
    QStringList paths = path.split("/", QString::SkipEmptyParts);
    QStringListIterator it(paths);
    if (it.hasNext())
    {
      QString rootEntry = it.next();
      // search the root templates
      if (d->rootTemplates.contains(rootEntry))
      {
        if (!dynamic_cast<ctkPluginGeneratorFolderTemplate*>(d->rootTemplates[rootEntry]))
        {
          throw std::exception("The segment \"" + rootEntry.toAscii() + "\" in \"" + path.toAscii() + "\" is not a folder");
        }
        parentTemplate = d->rootTemplates[rootEntry];
      }
      else
      {
        parentTemplate = new ctkPluginGeneratorFolderTemplate(rootEntry);
        d->rootTemplates.insert(rootEntry, parentTemplate);
      }
      while (it.hasNext())
      {
        QString currEntry = it.next();
        QListIterator<QObject*> children(parentTemplate->children());
        bool childFound = false;
        while (children.hasNext())
        {
          QObject* child = it.next();
          if (child->objectName() == currEntry)
          {
            childFound = true;
            parentTemplate = qobject_cast<ctkPluginGeneratorAbstractTemplate*>(child);
            if (parentTemplate == 0)
            {
              throw std::exception("The segment \"" + currEntry.toAscii() + "\" in \"" + path.toAscii() + "\" is not a folder");
            }
            break;
          }
        }

        if (!childFound)
        {
          parentTemplate = new ctkPluginGeneratorFolderTemplate(currEntry, parentTemplate);
        }
      }
    }

    templ->setParent(parentTemplate);
  }
}

void ctkPluginGeneratorCodeModel::create(const QString& location)
{
  Q_D(ctkPluginGeneratorCodeModel);

  QListIterator<ctkPluginGeneratorAbstractTemplate*> it(d->rootTemplates);
  while (it.hasNext())
  {
    ctkPluginGeneratorAbstractTemplate* templ = it.next();
    templ->create(location);
  }
}
