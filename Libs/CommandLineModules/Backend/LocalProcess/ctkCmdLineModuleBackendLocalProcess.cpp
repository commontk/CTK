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
#include "ctkCmdLineModuleTimeoutException.h"

#include "ctkUtils.h"
#include <iostream>
#include <QProcess>
#include <QUrl>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleBackendLocalProcessPrivate
{

  int m_TimeoutForXMLRetrieval;

  ctkCmdLineModuleBackendLocalProcessPrivate()
    : m_TimeoutForXMLRetrieval(0) // use the value from the module manager
  {
  }

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
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
            args = valuesIter.value().toString().split(',', Qt::SkipEmptyParts);
            #else
            args = valuesIter.value().toString().split(',', QString::SkipEmptyParts);
            #endif
          }
          else
          {
            args.push_back(valuesIter.value().toString());
          }

          if (args.length() > 0)
          {
            foreach(QString arg, args)
            {
              if (parameter.tag() == "string")
              {
                cmdLineArgs << argFlag << arg;
              }
              else
              {
                QString trimmedArg = arg.trimmed();
                if (trimmedArg.length() != 0) // If not string, and no arg, we don't output. We need this policy for integers, doubles, etc.
                {
                  cmdLineArgs << argFlag << trimmedArg;
                }
              }
            } // end foreach
          } // end if (args.length() > 0)
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
QByteArray ctkCmdLineModuleBackendLocalProcess::rawXmlDescription(const QUrl &location, int timeout)
{
  QProcess process;
  process.setReadChannel(QProcess::StandardOutput);
  process.start(location.toLocalFile(), QStringList("--xml"));

  if (!process.waitForFinished(timeout))
  {
    if (process.error() == QProcess::Timedout)
    {
      throw ctkCmdLineModuleTimeoutException(location, process.errorString());
    }
    else if (process.exitStatus() == QProcess::CrashExit ||
             process.error() != QProcess::UnknownError)
    {
      throw ctkCmdLineModuleRunException(location, process.exitCode(), process.errorString());
    }
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

//----------------------------------------------------------------------------
void ctkCmdLineModuleBackendLocalProcess::setTimeOutForXMLRetrieval(int timeOut)
{
  d->m_TimeoutForXMLRetrieval = timeOut;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleBackendLocalProcess::timeOutForXMLRetrieval() const
{
  return d->m_TimeoutForXMLRetrieval;
}
