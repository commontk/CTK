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

#ifndef CTKCMDLINEMODULEPROCESSTASK_H
#define CTKCMDLINEMODULEPROCESSTASK_H

#include "ctkCmdLineModuleXmlProgressWatcher.h"
#include "ctkCmdLineModuleFutureInterface.h"

#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QBuffer>

class QProcess;

class ctkCmdLineModuleProcessTask : public QObject, public ctkCmdLineModuleFutureInterface, public QRunnable
{
  Q_OBJECT

public:

  ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args);
  ~ctkCmdLineModuleProcessTask();

  ctkCmdLineModuleFuture start();

  void run();

protected Q_SLOTS:

  void pollCancelState();

private:

  QProcess* process;
  const QString location;
  const QStringList args;

};

class ctkCmdLineModuleProcessProgressWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleProcessProgressWatcher(QProcess& process, const QString& location,
                                         ctkCmdLineModuleFutureInterface& futureInterface);

protected Q_SLOTS:

  void filterStarted(const QString& name, const QString& comment);
  void filterProgress(float progress);
  void filterFinished(const QString& name);

  void filterXmlError(const QString& error);

private:

  int updateProgress(float progress);
  int incrementProgress();

  QProcess& process;
  QString location;
  ctkCmdLineModuleFutureInterface& futureInterface;
  ctkCmdLineModuleXmlProgressWatcher processXmlWatcher;
  int progressValue;
};

#endif // CTKCMDLINEMODULEPROCESSTASK_H
