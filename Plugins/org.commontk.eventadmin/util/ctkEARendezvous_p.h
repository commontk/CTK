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


#ifndef CTKEARENDEZVOUS_P_H
#define CTKEARENDEZVOUS_P_H

#include "ctkEACyclicBarrier_p.h"

#include <QAtomicInt>

/**
 * This is a simplified version of the ctkEACyclicBarrier implementation.
 * It provides the same methods but internally ignores the exceptions.
 */
class ctkEARendezvous : public ctkEACyclicBarrier
{
private:

  /** Flag for timedout handling. */
  mutable QAtomicInt timedout;

public:

  /**
   * Create a Barrier for the indicated number of parties, and the default
   * Rotator function to run at each barrier point.
   */
  ctkEARendezvous();

  /**
   * \see {@link ctkEACyclicBarrier#barrier()}
   */
  void waitForRendezvous();

  /**
   * \see {@link CyclicBarrier#attemptBarrier(long)}
   */
  void waitAttemptForRendezvous(long timeout);

};

#endif // CTKEARENDEZVOUS_P_H
