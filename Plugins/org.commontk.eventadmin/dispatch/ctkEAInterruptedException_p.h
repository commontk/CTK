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


#ifndef CTKEAINTERRUPTEDEXCEPTION_P_H
#define CTKEAINTERRUPTEDEXCEPTION_P_H

#include <ctkException.h>

/**
 * Thrown when a ctkEAInterruptibleThread is waiting, sleeping, or otherwise occupied,
 * and the thread is interrupted, either before or during the activity. Occasionally
 * a method may wish to test whether the current thread has been interrupted, and if so,
 * to immediately throw this exception. The following code can be used to achieve this effect:
 *
 * \code
 * if (ctkEAInterruptibleThread::interrupted())  // Clears interrupted status!
 *   throw ctkEAInterruptedException();
 * \endcode
 */
class ctkEAInterruptedException : public ctkRuntimeException
{
public:
  ctkEAInterruptedException();

  ~ctkEAInterruptedException() throw();

  const char* name() const throw();

  ctkEAInterruptedException* clone() const;

  void rethrow() const;
};

#endif // CTKEAINTERRUPTEDEXCEPTION_P_H
