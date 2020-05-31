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


#include "ctkPluginGeneratorCodeModel.h"

#include <QDir>
#include <QHash>

#include <ctkException.h>


class ctkPluginGeneratorCodeModelPrivate
{
public:

  QHash<QString, ctkPluginGeneratorAbstractTemplate*> rootTemplates;
  QHash<QString, QString> contentMap;

  QString symbolicNameWithPeriods;
  QString symbolicNameWithUnderscores;
  QString exportMacro;
  QString exportMacroInclude;
  QString license;
};

class ctkPluginGeneratorFolderTemplate : public ctkPluginGeneratorAbstractTemplate
{
public:

  ctkPluginGeneratorFolderTemplate(const QString& name, ctkPluginGeneratorAbstractTemplate* parent = 0)
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
      throw ctkRuntimeException("The directory " + location + this->objectName() + " could not be created");
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

void ctkPluginGeneratorCodeModel::addContent(const QString& marker, const QString& content)
{
  Q_D(ctkPluginGeneratorCodeModel);
  d->contentMap[marker] = content;
}

QString ctkPluginGeneratorCodeModel::getContent(const QString& marker) const
{
  Q_D(const ctkPluginGeneratorCodeModel);
  if (d->contentMap.contains(marker))
  {
    return d->contentMap[marker];
  }
  return QString();
}

void ctkPluginGeneratorCodeModel::setExportMacro(const QString& exportMacro)
{
  Q_D(ctkPluginGeneratorCodeModel);
  d->exportMacro = exportMacro;
}

QString ctkPluginGeneratorCodeModel::getExportMacro() const
{
  Q_D(const ctkPluginGeneratorCodeModel);
  return d->exportMacro;
}

void ctkPluginGeneratorCodeModel::setExportMacroInclude(const QString& exportMacroInclude)
{
  Q_D(ctkPluginGeneratorCodeModel);
  d->exportMacroInclude = exportMacroInclude;
}

QString ctkPluginGeneratorCodeModel::getExportMacroInclude() const
{
  Q_D(const ctkPluginGeneratorCodeModel);
  return d->exportMacroInclude;
}

void ctkPluginGeneratorCodeModel::setSymbolicName(const QString& symbolicName)
{
  Q_D(ctkPluginGeneratorCodeModel);
  d->symbolicNameWithPeriods = QString(symbolicName).replace("_", ".");
  d->symbolicNameWithUnderscores = QString(symbolicName).replace(".", "_");
}

QString ctkPluginGeneratorCodeModel::getSymbolicName(bool withPeriods) const
{
  Q_D(const ctkPluginGeneratorCodeModel);
  if (withPeriods)
  {
    return d->symbolicNameWithPeriods;
  }
 return d->symbolicNameWithUnderscores;
}

void ctkPluginGeneratorCodeModel::setLicense(const QString& license)
{
  Q_D(ctkPluginGeneratorCodeModel);
  d->license = license;
}

QString ctkPluginGeneratorCodeModel::getLicense() const
{
  Q_D(const ctkPluginGeneratorCodeModel);
  return d->license;
}

void ctkPluginGeneratorCodeModel::addTemplate(ctkPluginGeneratorAbstractTemplate *templ,
                                              const QString& path)
{
  Q_D(ctkPluginGeneratorCodeModel);

  templ->setCodeModel(this);

  if (path.isEmpty())
  {
    d->rootTemplates.insert(templ->objectName(), templ);
    templ->setParent(this);
  }
  else
  {
    ctkPluginGeneratorAbstractTemplate* parentTemplate = 0;
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QStringList paths = path.split("/", Qt::SkipEmptyParts);
    #else
    QStringList paths = path.split("/", QString::SkipEmptyParts);
    #endif
    QStringListIterator it(paths);
    if (it.hasNext())
    {
      QString rootEntry = it.next();
      // search the root templates
      if (d->rootTemplates.contains(rootEntry))
      {
        if (!dynamic_cast<ctkPluginGeneratorFolderTemplate*>(d->rootTemplates[rootEntry]))
        {
          throw ctkRuntimeException("The segment \"" + rootEntry + "\" in \"" + path + "\" is not a folder");
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
          QObject* child = children.next();
          if (child->objectName() == currEntry)
          {
            childFound = true;
            parentTemplate = qobject_cast<ctkPluginGeneratorAbstractTemplate*>(child);
            if (parentTemplate == 0)
            {
              throw ctkRuntimeException("The segment \"" + currEntry + "\" in \"" + path + "\" is not a folder");
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

ctkPluginGeneratorAbstractTemplate* ctkPluginGeneratorCodeModel::getTemplate(const QString& path) const
{
  Q_D(const ctkPluginGeneratorCodeModel);

  if (!path.contains("/"))
  {
    foreach(ctkPluginGeneratorAbstractTemplate* t, d->rootTemplates)
    {
      if (t->objectName() == path) return t;
      ctkPluginGeneratorAbstractTemplate* child =
          t->findChild<ctkPluginGeneratorAbstractTemplate*>(path);
      if (child) return child;
    }
    return 0;
  }

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QStringList paths = path.split("/", Qt::SkipEmptyParts);
  #else
  QStringList paths = path.split("/", QString::SkipEmptyParts);
  #endif
  if (paths.empty()) return 0;

  QObject* currChild = d->rootTemplates[paths.front()];
  paths.pop_front();

  int depth = 0;
  foreach (QString curr, paths)
  {
    foreach (QObject* child, currChild->children())
    {
      if (child->objectName() == curr)
      {
        currChild = child;
        ++depth;
        break;
      }
    }
  }

  if (paths.size() == depth)
  {
    return qobject_cast<ctkPluginGeneratorAbstractTemplate*>(currChild);
  }

  return 0;
}

void ctkPluginGeneratorCodeModel::create(const QString& location)
{
  Q_D(ctkPluginGeneratorCodeModel);

  QListIterator<ctkPluginGeneratorAbstractTemplate*> it(d->rootTemplates.values());
  while (it.hasNext())
  {
    ctkPluginGeneratorAbstractTemplate* templ = it.next();
    templ->create(location);
  }
}

void ctkPluginGeneratorCodeModel::reset()
{
  Q_D(ctkPluginGeneratorCodeModel);
  qDeleteAll(d->rootTemplates.values());
  d->rootTemplates.clear();
}
