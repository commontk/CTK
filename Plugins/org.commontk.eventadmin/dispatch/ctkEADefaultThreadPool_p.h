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


#ifndef CTKEADEFAULTTHREADPOOL_P_H
#define CTKEADEFAULTTHREADPOOL_P_H

#include "ctkEAPooledExecutor_p.h"

/**
 * A thread pool that allows to execute tasks using pooled threads in order
 * to ease the thread creation overhead.
 */
class ctkEADefaultThreadPool : public ctkEAPooledExecutor
{

public:

  /**
   * Create a new pool.
   */
  ctkEADefaultThreadPool(int poolSize, bool syncThreads);

  /**
   * Configure a new pool size.
   */
  void configure(int poolSize);

  /**
   * Close the pool i.e, stop pooling threads. Note that subsequently, task will
   * still be executed but no pooling is taking place anymore.
   */
  void close();

  /**
   * Execute the task in a free thread or create a new one.
   * @param task The task to execute
   */
  void executeTask(ctkEARunnable* task);

};

#endif // CTKEADEFAULTTHREADPOOL_P_H
