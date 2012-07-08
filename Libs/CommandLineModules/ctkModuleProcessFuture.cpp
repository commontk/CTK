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

#include "ctkModuleProcessFuture.h"

struct ctkModuleProcessFutureInterfacePrivate
{
  ctkModuleProcessFutureInterfacePrivate()
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

ctkModuleProcessFutureInterface::QFutureInterface(State initialState)
  : QFutureInterfaceBase(initialState), d(new ctkModuleProcessFutureInterfacePrivate)
{ }

ctkModuleProcessFutureInterface::QFutureInterface(const ctkModuleProcessFutureInterface& other)
  : QFutureInterfaceBase(other), d(other.d)
{
  d->refCount.ref();
}

ctkModuleProcessFutureInterface ctkModuleProcessFutureInterface::canceledResult()
{ return ctkModuleProcessFutureInterface(State(Started | Finished | Canceled)); }

ctkModuleProcessFutureInterface& ctkModuleProcessFutureInterface::operator=(const ctkModuleProcessFutureInterface& other)
{
  QFutureInterfaceBase::operator=(other);
  other.d->refCount.ref();
  if(!d->refCount.deref()) delete d;
  d = other.d;
  return *this;
}

int ctkModuleProcessFutureInterface::exitCode() const
{ QMutexLocker lock(this->mutex()); return d->_exitCode; }

void ctkModuleProcessFutureInterface::reportExitCode(int code)
{ QMutexLocker lock(this->mutex()); d->_exitCode = code; }

QProcess::ExitStatus ctkModuleProcessFutureInterface::exitStatus() const
{ QMutexLocker lock(this->mutex()); return d->_exitStatus; }

void ctkModuleProcessFutureInterface::reportExitStatus(QProcess::ExitStatus status)
{ QMutexLocker lock(this->mutex()); d->_exitStatus = status; }

QProcess::ProcessError ctkModuleProcessFutureInterface::error() const
{ QMutexLocker lock(this->mutex()); return d->_processError; }

void ctkModuleProcessFutureInterface::reportProcessError(QProcess::ProcessError procErr)
{ QMutexLocker lock(this->mutex()); d->_processError = procErr; }

QString ctkModuleProcessFutureInterface::errorString() const
{ QMutexLocker lock(this->mutex()); return d->_errorString; }

void ctkModuleProcessFutureInterface::reportErrorString(const QString& errorStr)
{ QMutexLocker lock(this->mutex()); d->_errorString = errorStr; }

QString ctkModuleProcessFutureInterface::standardOutput() const
{ QMutexLocker lock(this->mutex()); return d->_stdOut; }

void ctkModuleProcessFutureInterface::reportStandardOutput(const QString& stdOut)
{ QMutexLocker lock(this->mutex()); d->_stdOut = stdOut; }

QString ctkModuleProcessFutureInterface::standardError() const
{ QMutexLocker lock(this->mutex()); return d->_stdErr; }

void ctkModuleProcessFutureInterface::reportStandardError(const QString& stdErr)
{ QMutexLocker lock(this->mutex()); d->_stdErr = stdErr; }
