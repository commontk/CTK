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


#ifndef CTKEASYNCMASTERTHREAD_P_H
#define CTKEASYNCMASTERTHREAD_P_H

#include "ctkEAInterruptibleThread_p.h"


/**
 * Used to execute synchronuous tasks. This is not done
 * in the main thread because we need to be able to
 * interrupt the thread waiting on a synchronuous task
 * (for example on a timeout). ctkEAInterruptibleThread
 * provides this capability.
 */
class ctkEASyncMasterThread : public QObject
{
  Q_OBJECT

public:
  ctkEASyncMasterThread();

  void syncRun(QRunnable* command);

  void stop();

public Q_SLOTS:

  void runCommand();

private:

  ctkEAInterruptibleThread thread;
  QRunnable* command;
  QMutex mutex;
  QWaitCondition waitCond;
};

#endif // CTKEASYNCMASTERTHREAD_P_H
