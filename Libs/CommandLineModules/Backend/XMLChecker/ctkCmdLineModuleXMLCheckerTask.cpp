/*=============================================================================

  Library: CTK

  Copyright (c) University College London

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

#include "ctkCmdLineModuleXMLCheckerTask_p.h"
#include "ctkCmdLineModuleFuture.h"

#include <QThreadPool>

//----------------------------------------------------------------------------
ctkCmdLineModuleXMLCheckerTask::ctkCmdLineModuleXMLCheckerTask()
{
}


//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleXMLCheckerTask::start()
{
  this->setRunnable(this);
  this->setProgressRange(0,0);
  this->reportStarted();

  ctkCmdLineModuleFuture future = this->future();
  QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);

  return future;
}


//----------------------------------------------------------------------------
void ctkCmdLineModuleXMLCheckerTask::run()
{
  if (this->isCanceled())
  {
    this->reportFinished();
    return;
  }

  // Actually nothing to do, as we don't actually run the process.

  // Report a successful finish.
  this->setProgressRange(0,1);
  this->setProgressValue(1);
  this->reportFinished();
}
