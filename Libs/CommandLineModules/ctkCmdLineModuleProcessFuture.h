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

#ifndef CTKCMDLINEMODULEPROCESSFUTURE_H
#define CTKCMDLINEMODULEPROCESSFUTURE_H

#include <ctkCommandLineModulesExport.h>

#include <QFutureInterface>
#include <QFutureWatcher>
#include <QProcess>

class ctkCmdLineModuleProcessFutureInterfacePrivate;

struct ctkCmdLineModuleProcessDummy {};
typedef QFutureWatcher<ctkCmdLineModuleProcessDummy> ctkCmdLineModuleProcessFutureWatcher;


template<>
class CTK_CMDLINEMODULE_EXPORT QFutureInterface<ctkCmdLineModuleProcessDummy> : public QFutureInterfaceBase
{

public:

  QFutureInterface(State initialState = NoState);
  QFutureInterface(const QFutureInterface& other);

  static QFutureInterface canceledResult();

  QFutureInterface& operator=(const QFutureInterface& other);

  int exitCode() const;
  void reportExitCode(int code);

  QProcess::ExitStatus exitStatus() const;
  void reportExitStatus(QProcess::ExitStatus status);

  QProcess::ProcessError error() const;
  void reportProcessError(QProcess::ProcessError procErr);

  QString errorString() const;
  void reportErrorString(const QString& errorStr);

  QString standardOutput() const;
  void reportStandardOutput(const QString& stdOut);

  QString standardError() const;
  void reportStandardError(const QString& stdErr);

private:

  ctkCmdLineModuleProcessFutureInterfacePrivate* d;
};

typedef QFutureInterface<ctkCmdLineModuleProcessDummy> ctkCmdLineModuleProcessFutureInterface;


template<>
class QFuture<ctkCmdLineModuleProcessDummy>
{

public:

  QFuture()
    : d(ctkCmdLineModuleProcessFutureInterface::canceledResult())
  { }

  explicit QFuture(ctkCmdLineModuleProcessFutureInterface *p) // internal
    : d(*p)
  { }

  QFuture(const QFuture &other)
    : d(other.d)
  { }

  ~QFuture()
  { }

  QFuture& operator=(const QFuture& other);
  bool operator==(const QFuture& other) const { return (d == other.d); }
  bool operator!=(const QFuture& other) const { return (d != other.d); }

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

  friend class QFutureWatcher<ctkCmdLineModuleProcessDummy>;

  mutable ctkCmdLineModuleProcessFutureInterface d;
};

typedef QFuture<ctkCmdLineModuleProcessDummy> ctkCmdLineModuleProcessFuture;


inline ctkCmdLineModuleProcessFuture& ctkCmdLineModuleProcessFuture::operator=(const ctkCmdLineModuleProcessFuture& other)
{
  d = other.d;
  return *this;
}

#endif // CTKCMDLINEMODULEPROCESSFUTURE_H
