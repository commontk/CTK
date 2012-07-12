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

#include "ctkCmdLineModuleFuture.h"

struct ctkCmdLineModuleFutureInterfacePrivate
{
  ctkCmdLineModuleFutureInterfacePrivate()
    : refCount(1), _exitCode(0), _exitStatus(QProcess::NormalExit),
      _processError(QProcess::UnknownError)
  {}

  QAtomicInt refCount;

  int _exitCode;
  QProcess::ExitStatus _exitStatus;
  QProcess::ProcessError _processError;
  QString _errorString;
  QString _stdOut;
  QString _stdErr;
};

ctkCmdLineModuleFutureInterface::QFutureInterface(State initialState)
  : QFutureInterfaceBase(initialState), d(new ctkCmdLineModuleFutureInterfacePrivate)
{ }

ctkCmdLineModuleFutureInterface::QFutureInterface(const ctkCmdLineModuleFutureInterface& other)
  : QFutureInterfaceBase(other), d(other.d)
{
  d->refCount.ref();
}

ctkCmdLineModuleFutureInterface ctkCmdLineModuleFutureInterface::canceledResult()
{ return ctkCmdLineModuleFutureInterface(State(Started | Finished | Canceled)); }

ctkCmdLineModuleFutureInterface& ctkCmdLineModuleFutureInterface::operator=(const ctkCmdLineModuleFutureInterface& other)
{
  QFutureInterfaceBase::operator=(other);
  other.d->refCount.ref();
  if(!d->refCount.deref()) delete d;
  d = other.d;
  return *this;
}

int ctkCmdLineModuleFutureInterface::exitCode() const
{ QMutexLocker lock(this->mutex()); return d->_exitCode; }

void ctkCmdLineModuleFutureInterface::reportExitCode(int code)
{ QMutexLocker lock(this->mutex()); d->_exitCode = code; }

QProcess::ExitStatus ctkCmdLineModuleFutureInterface::exitStatus() const
{ QMutexLocker lock(this->mutex()); return d->_exitStatus; }

void ctkCmdLineModuleFutureInterface::reportExitStatus(QProcess::ExitStatus status)
{ QMutexLocker lock(this->mutex()); d->_exitStatus = status; }

QProcess::ProcessError ctkCmdLineModuleFutureInterface::error() const
{ QMutexLocker lock(this->mutex()); return d->_processError; }

void ctkCmdLineModuleFutureInterface::reportProcessError(QProcess::ProcessError procErr)
{ QMutexLocker lock(this->mutex()); d->_processError = procErr; }

QString ctkCmdLineModuleFutureInterface::errorString() const
{ QMutexLocker lock(this->mutex()); return d->_errorString; }

void ctkCmdLineModuleFutureInterface::reportErrorString(const QString& errorStr)
{ QMutexLocker lock(this->mutex()); d->_errorString = errorStr; }

QString ctkCmdLineModuleFutureInterface::standardOutput() const
{ QMutexLocker lock(this->mutex()); return d->_stdOut; }

void ctkCmdLineModuleFutureInterface::reportStandardOutput(const QString& stdOut)
{ QMutexLocker lock(this->mutex()); d->_stdOut = stdOut; }

QString ctkCmdLineModuleFutureInterface::standardError() const
{ QMutexLocker lock(this->mutex()); return d->_stdErr; }

void ctkCmdLineModuleFutureInterface::reportStandardError(const QString& stdErr)
{ QMutexLocker lock(this->mutex()); d->_stdErr = stdErr; }
