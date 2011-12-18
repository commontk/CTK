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

#ifndef CTKMODULEPROCESSRUNNER_P_H
#define CTKMODULEPROCESSRUNNER_P_H

#include <QObject>
#include <QProcess>

#include "ctkModuleProcessFuture.h"

class ctkModuleProcessRunner : public QObject, public QRunnable, public ctkModuleProcessFutureInterface
{
  Q_OBJECT

public:

  ctkModuleProcessRunner(const QString& location, const QStringList& args);

  ctkModuleProcessFuture start();

  void run();

protected Q_SLOTS:

  void processStarted();

  void processFinished(int exitCode, QProcess::ExitStatus status);

  void processError(QProcess::ProcessError);

private:

  QProcess process;
  const QString location;
  const QStringList args;
};

#endif // CTKMODULEPROCESSRUNNER_P_H
