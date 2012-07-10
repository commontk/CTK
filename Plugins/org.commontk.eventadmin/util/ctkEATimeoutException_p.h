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


#ifndef CTKEATIMEOUTEXCEPTION_P_H
#define CTKEATIMEOUTEXCEPTION_P_H

#include <ctkException.h>

/**
 * Thrown by synchronization classes that report
 * timeouts via exceptions.
 **/
class ctkEATimeoutException : public ctkRuntimeException
{

public:

  /**
   * The approximate time that the operation lasted before
   * this timeout exception was thrown.
   **/
  const long duration;

  /**
   * Constructs a TimeoutException with the
   * specified duration value and detail message.
   */
  ctkEATimeoutException(long time, const QString& message = QString());

  ~ctkEATimeoutException() throw();

  /**
   * @see ctkException::name()
   */
  const char* name() const throw();

  /**
   * @see ctkException::clone()
   */
  ctkEATimeoutException* clone() const;

  /**
   * @see ctkException::rethrow()
   */
  void rethrow() const;
};

#endif // CTKEATIMEOUTEXCEPTION_P_H
