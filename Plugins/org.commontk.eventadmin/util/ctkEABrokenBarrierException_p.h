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


#ifndef CTKEABROKENBARRIEREXCEPTION_P_H
#define CTKEABROKENBARRIEREXCEPTION_P_H

#include <ctkException.h>

/**
 * Thrown by barrier classes upon interruption of participant threads
 **/
class ctkEABrokenBarrierException : public ctkRuntimeException
{

public:

  /**
   * The index that barrier would have returned upon
   * normal return;
   **/
  const int index;

  /**
   * Constructs a BrokenBarrierException with the
   * specified index and detail message.
   */
  ctkEABrokenBarrierException(int idx, const QString& message = QString());

  ~ctkEABrokenBarrierException() throw();

  /**
   * @see ctkException::name()
   */
  const char* name() const throw();

  /**
   * @see ctkException::clone()
   */
  ctkEABrokenBarrierException* clone() const;

  /**
   * @see ctkException::rethrow()
   */
  void rethrow() const;
};

#endif // CTKEABROKENBARRIEREXCEPTION_P_H
