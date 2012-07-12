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

#include "ctkCmdLineModuleProcess_p.h"


ctkCmdLineModuleProcess::ctkCmdLineModuleProcess(const QString& location, const QStringList& args)
  : process(), location(location), args(args)
{
}

ctkCmdLineModuleFuture ctkCmdLineModuleProcess::start()
{
  this->reportStarted();
  ctkCmdLineModuleFuture future(this);
  connect(&process, SIGNAL(started()), this, SLOT(processStarted()));
  connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
  connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));

  process.start(location, args);
  return future;
}

void ctkCmdLineModuleProcess::processStarted()
{
  qDebug() << "Reporting process started";
  this->reportStarted();
}

void ctkCmdLineModuleProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
  Q_UNUSED(exitCode)
  Q_UNUSED(status)
  qDebug() << "Reporting process finished";
  this->reportExitCode(exitCode);
  this->reportExitStatus(status);
  this->reportProcessError(process.error());
  this->reportErrorString(process.errorString());
  this->reportStandardOutput(process.readAllStandardOutput());
  this->reportStandardError(process.readAllStandardError());
  this->reportFinished();
}

void ctkCmdLineModuleProcess::processError(QProcess::ProcessError)
{
  //qDebug() << "Reporting process error";
  //this->reportException(ctkCmdLineModuleProcessException(process.errorString(), process.exitCode(),
  //                                                process.exitStatus()));
}
