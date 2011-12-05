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


#include "ctkEARendezvous_p.h"

#include "ctkEABrokenBarrierException_p.h"
#include "ctkEATimeoutException_p.h"

ctkEARendezvous::ctkEARendezvous()
  : ctkEACyclicBarrier(2), timedout()
{
}

void ctkEARendezvous::waitForRendezvous()
{
  if (timedout.fetchAndAddOrdered(0))
  {
    // if we have timed out, we return immediately
    return;
  }
  try
  {
    this->barrier();
  }
  catch (const ctkEABrokenBarrierException&)
  {
  }
}

void ctkEARendezvous::waitAttemptForRendezvous(long timeout)
{
  try
  {
    this->attemptBarrier(timeout);
    this->restart();
  }
  catch (const ctkEABrokenBarrierException&)
  {
  }
  catch (const ctkEATimeoutException& te)
  {
    timedout.testAndSetOrdered(0, 1);
    throw te;
  }
}
