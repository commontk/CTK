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

#ifndef CTKCMDLINEMODULEFUTURE_H
#define CTKCMDLINEMODULEFUTURE_H

#include <ctkCommandLineModulesCoreExport.h>

//#include <QFutureInterface>
//#include <QFutureWatcher>
#include <QProcess>

/**
 * \ingroup CommandLineModulesCore
 */
class ctkCmdLineModuleFuture
{

public:

  ctkCmdLineModuleFuture()
    : d(ctkCmdLineModuleFutureInterface::canceledResult())
  { }

  explicit ctkCmdLineModuleFuture(const ctkCmdLineModuleProcess& p) // internal
    : d(*p)
  { }

  ctkCmdLineModuleFuture(const ctkCmdLineModuleFuture &other)
    : d(other.d)
  { }

  ~ctkCmdLineModuleFuture()
  { }

  ctkCmdLineModuleFuture& operator=(const ctkCmdLineModuleFuture& other);
  bool operator==(const ctkCmdLineModuleFuture& other) const { return (d == other.d); }
  bool operator!=(const ctkCmdLineModuleFuture& other) const { return (d != other.d); }

  void cancel() { d.cancel(); }
  bool isCanceled() const { return d.isCanceled(); }

  bool isStarted() const { return d.isStarted(); }
  bool isFinished() const { return d.isFinished(); }
  bool isRunning() const { return d.isRunning(); }

  int exitCode() const { return d.exitCode(); }
  int exitStatus() const { return d.exitStatus(); }
  QProcess::ProcessError error() const { return d.error(); }
  QString errorString() const { return d.errorString(); }

  QString standardOutput() const { return d.standardOutput(); }
  QString standardError() const { return d.standardError(); }

  int progressValue() const { return d.progressValue(); }
  int progressMinimum() const { return d.progressMinimum(); }
  int progressMaximum() const { return d.progressMaximum(); }
  QString progressText() const { return d.progressText(); }
  void waitForFinished() { d.waitForFinished(); }

private:

  friend class ctkCmdLineModuleFutureWatcher;

  mutable ctkCmdLineModuleProcess d;
};


inline ctkCmdLineModuleFuture& ctkCmdLineModuleFuture::operator=(const ctkCmdLineModuleFuture& other)
{
  d = other.d;
  return *this;
}

#endif // CTKCMDLINEMODULEFUTURE_H
