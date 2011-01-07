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


#ifndef CTKEATHREADFACTORYUSER_P_H
#define CTKEATHREADFACTORYUSER_P_H

#include <QMutex>

#include "ctkEAThreadFactory_p.h"
#include "ctkEAInterruptibleThread_p.h"

/**
 * Base class for Executors and related classes that rely on thread factories.
 * Generally intended to be used as a mixin-style abstract class, but
 * can also be used stand-alone.
 */
class ctkEAThreadFactoryUser
{

protected:

  mutable QMutex mutex;

  ctkEAThreadFactory* threadFactory;

  class DefaultThread : public ctkEAInterruptibleThread
  {
  public:

    DefaultThread(ctkEARunnable* command);

    void run();

  private:

    ctkEARunnable* command;
    bool deleteCmd;
  };

  class DefaultThreadFactory : public ctkEAThreadFactory
  {
  public:

    ctkEAInterruptibleThread* newThread(ctkEARunnable* command);
  };

  ctkEAThreadFactoryUser();

  ~ctkEAThreadFactoryUser();

  /**
   * Set the factory for creating new threads.
   * By default, new threads are created without any special priority,
   * threadgroup, or status parameters.
   * You can use a different factory
   * to change the kind of Thread class used or its construction
   * parameters.
   * @param factory the factory to use
   * @return the previous factory
   */
  ctkEAThreadFactory* setThreadFactory(ctkEAThreadFactory* factory);

  /**
   * Get the factory for creating new threads.
   */
  ctkEAThreadFactory* getThreadFactory();

};

#endif // CTKEATHREADFACTORYUSER_P_H
