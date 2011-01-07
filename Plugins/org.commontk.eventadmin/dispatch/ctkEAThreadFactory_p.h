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


#ifndef CTKEATHREADFACTORY_P_H
#define CTKEATHREADFACTORY_P_H

class ctkEAInterruptibleThread;
class ctkEARunnable;

/**
 * Interface describing any class that can generate
 * new ctkEAInterruptibleThread objects. Using ctkEAThreadFactory removes
 * hardwiring of calls to <code>new ctkEAInterruptibleThread</code>, enabling
 * applications to use special thread subclasses, default
 * prioritization settings, etc.
 */
struct ctkEAThreadFactory
{
  virtual ~ctkEAThreadFactory() {}

  /**
   * Create a new thread that will run the given command when started.
   * The thread will delete the command after running it if deleteCmd is true.
   **/
  virtual ctkEAInterruptibleThread* newThread(ctkEARunnable* command) = 0;
};

#endif // CTKEATHREADFACTORY_P_H
