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

#include "ctkCmdLineModuleBackendLocalProcess.h"

#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleFrontend.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup.h"
#include "ctkCmdLineModuleProcessTask.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleRunException.h"

#include "ctkUtils.h"

#include <QProcess>
#include <QUrl>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleBackendLocalProcessPrivate
{

  QString normalizeFlag(const QString& flag) const
  {
    return flag.trimmed().remove(QRegExp("^-*"));
  }

  QStringList commandLineArguments(const QHash<QString,QVariant>& currentValues,
                                   const ctkCmdLineModuleDescription& description) const
  {
    QStringList cmdLineArgs;
    QHash<int, QString> indexedArgs;

    QHashIterator<QString,QVariant> valuesIter(currentValues);
    while(valuesIter.hasNext())
    {
      valuesIter.next();
      ctkCmdLineModuleParameter parameter = description.parameter(valuesIter.key());
      if (parameter.index() > -1)
      {
        indexedArgs.insert(parameter.index(), valuesIter.value().toString());
      }
      else
      {
        QString argFlag;
        if (parameter.longFlag().isEmpty())
        {
          argFlag = QString("-") + this->normalizeFlag(parameter.flag());
        }
        else
        {
          argFlag = QString("--") + this->normalizeFlag(parameter.longFlag());
        }

        if (parameter.tag() == "boolean")
        {
          if (valuesIter.value().toBool())
          {
            cmdLineArgs << argFlag;
          }
        }
        else
        {
          QStringList args;
          if (parameter.multiple())
          {
            args = valuesIter.value().toString().split(',', QString::SkipEmptyParts);
          }
          else
          {
            QString arg = valuesIter.value().toString().trimmed();
            if (arg.isEmpty())
            {
              arg = parameter.defaultValue().trimmed();
            }
            if (!arg.isEmpty())
            {
              args.push_back(valuesIter.value().toString());
            }
          }

          if (args.length() > 0) // don't write the argFlag if there was no argument, and no default.
          {
            foreach(QString arg, args)
            {
              cmdLineArgs << argFlag << arg;
            }
          }
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
};

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendLocalProcess::ctkCmdLineModuleBackendLocalProcess()
  : d(new ctkCmdLineModuleBackendLocalProcessPrivate){
}

//----------------------------------------------------------------------------
ctkCmdLineModuleBackendLocalProcess::~ctkCmdLineModuleBackendLocalProcess()
{
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendLocalProcess::name() const
{
  return "Local Process";
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendLocalProcess::description() const
{
  return "Runs an executable command line module using a local process.";
}

//----------------------------------------------------------------------------
QList<QString> ctkCmdLineModuleBackendLocalProcess::schemes() const
{
  static QList<QString> supportedSchemes = QList<QString>() << "file";
  return supportedSchemes;
}

//----------------------------------------------------------------------------
qint64 ctkCmdLineModuleBackendLocalProcess::timeStamp(const QUrl &location) const
{
  QFileInfo fileInfo(location.toLocalFile());
  if (fileInfo.exists())
  {
    QDateTime dateTime = fileInfo.lastModified();
    return ctk::msecsTo(QDateTime::fromTime_t(0), dateTime);
  }
  return 0;
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleBackendLocalProcess::rawXmlDescription(const QUrl &location)
{
  QProcess process;
  process.setReadChannel(QProcess::StandardOutput);
  process.start(location.toLocalFile(), QStringList("--xml"));

  if (!process.waitForFinished() || process.exitStatus() == QProcess::CrashExit ||
      process.error() != QProcess::UnknownError)
  {
    throw ctkCmdLineModuleRunException(location, process.exitCode(), process.errorString());
  }

  process.waitForReadyRead();
  return process.readAllStandardOutput();
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleBackendLocalProcess::run(ctkCmdLineModuleFrontend* frontend)
{
  QStringList args = d->commandLineArguments(frontend->values(), frontend->moduleReference().description());

  // Instances of ctkCmdLineModuleProcessTask are auto-deleted by the
  // thread pool.
  ctkCmdLineModuleProcessTask* moduleProcess =
      new ctkCmdLineModuleProcessTask(frontend->location().toLocalFile(), args);
  return moduleProcess->start();
}
