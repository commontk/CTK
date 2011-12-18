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

#include "ctkModuleManager.h"

#include "ctkModuleDescriptionValidator.h"
#include "ctkModuleObjectHierarchyReader.h"
#include "ctkModuleProcessRunner_p.h"
#include "ctkModuleReferencePrivate.h"

#include <QStringList>
#include <QBuffer>

#include <QProcess>
#include <QFuture>

QString normalizeFlag(const QString& flag)
{
  return flag.trimmed().remove(QRegExp("^-*"));
}

ctkModuleManager::ctkModuleManager(ctkModuleDescriptionFactory *descriptionFactory)
  : descriptionFactory(descriptionFactory)
{
}

QStringList ctkModuleManager::createCommandLineArgs(QObject *hierarchy)
{
  ctkModuleObjectHierarchyReader reader(hierarchy);

  QStringList cmdLineArgs;
  QHash<int, QString> indexedArgs;
  while(reader.readNextParameter())
  {
    if (reader.index() > -1)
    {
      indexedArgs.insert(reader.index(), reader.value());
    }
    else
    {
      QString argFlag;
      if (reader.longFlag().isEmpty())
      {
        argFlag = QString("-") + normalizeFlag(reader.flag());
      }
      else
      {
        argFlag = QString("--") + normalizeFlag(reader.longFlag());
      }

      QStringList args;
      if (reader.isMultiple())
      {
        args = reader.value().split(',', QString::SkipEmptyParts);
      }
      else
      {
        args.push_back(reader.value());
      }

      foreach(QString arg, args)
      {
        cmdLineArgs << argFlag << arg;
      }
    }
  }

  QList<int> indexes = indexedArgs.keys();
  qSort(indexes.begin(), indexes.end());
  foreach(int index, indexes)
  {
    cmdLineArgs << indexedArgs[index];
  }

  return cmdLineArgs;
}

ctkModuleReference ctkModuleManager::addModule(const QString& location)
{
  QProcess process;
  process.setReadChannel(QProcess::StandardOutput);
  process.start(location, QStringList("--xml"));

  ctkModuleReference ref;
  ref.d->loc = location;
  if (!process.waitForFinished() || process.exitStatus() == QProcess::CrashExit ||
      process.error() != QProcess::UnknownError)
  {
    qWarning() << "The executable at" << location << "could not be started:" << process.errorString();
    return ref;
  }

  process.waitForReadyRead();
  QByteArray xml = process.readAllStandardOutput();

  qDebug() << xml;

  // validate the outputted xml description
  QBuffer input(&xml);
  input.open(QIODevice::ReadOnly);

  ctkModuleDescriptionValidator validator(&input);
  if (!validator.validateXMLInput())
  {
    qCritical() << validator.errorString();
    return ref;
  }

  ref.d->xml = xml;
  ref.d->objectRepresentation = descriptionFactory->createObjectRepresentationFromXML(ref.d->xml);
  ref.d->setGUI(descriptionFactory->createGUIFromXML(ref.d->xml));

  cache[location] = ref;
  return ref;
}

ctkModuleProcessFuture ctkModuleManager::run(const ctkModuleReference& moduleRef)
{
  // TODO: manage memory
  QStringList args = createCommandLineArgs(moduleRef.d->objectRepresentation);
  ctkModuleProcessRunner* moduleProcess = new ctkModuleProcessRunner(moduleRef.location(), args);
  return moduleProcess->start();
}
