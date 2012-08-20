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

#include "ctkCmdLineModuleFunctionPointerTask_p.h"

#include "ctkCmdLineModuleBackendFPDescriptionPrivate.h"

#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleRunException.h"

//----------------------------------------------------------------------------
ctkCmdLineModuleFunctionPointerTask::ctkCmdLineModuleFunctionPointerTask(const ctkCmdLineModuleBackendFunctionPointer::Description &fpDescr, const QList<QVariant> &paramValues)
  : FpDescription(fpDescr)
  , ParamValues(paramValues)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleFunctionPointerTask::start()
{
  this->setRunnable(this);
  this->setProgressRange(0,0);
  this->reportStarted();
  ctkCmdLineModuleFuture future = this->future();
  QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
  return future;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFunctionPointerTask::run()
{
  if (this->isCanceled())
  {
    this->reportFinished();
    return;
  }

  // call the function pointer and catch any exceptions
  QString excMsg;
  try
  {
    FpDescription.d->FpProxy.call(ParamValues);
  }
  catch (const std::exception& e)
  {
    excMsg = e.what();
  }
  catch (...)
  {
    excMsg = "Unknown exception.";
  }

  if (!excMsg.isNull())
  {
    this->reportException(ctkCmdLineModuleRunException(FpDescription.moduleLocation(), 0, excMsg));
  }

  this->setProgressRange(0,1);
  this->setProgressValue(1);

  //this->reportResult(result);
  this->reportFinished();
}
