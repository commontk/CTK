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

#include <QFutureInterface>

#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QProcess>

class ctkCmdLineModuleProcessTask : public QObject, public QFutureInterface<QString>, public QRunnable
{
  Q_OBJECT

public:

  ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args);
  ~ctkCmdLineModuleProcessTask();

  QFuture<QString> start();

  void run();

protected Q_SLOTS:

  void error(QProcess::ProcessError error);
  void readyReadStandardError();
  void readyReadStandardOutput();

private:

  const QString location;
  const QStringList args;
  QString result;
};

#endif // CTKCMDLINEMODULEPROCESSTASK_H
