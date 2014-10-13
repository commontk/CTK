/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#include <ctkHighPrecisionTimer.h>

#include <QDebug>
#include <QTest>


//-----------------------------------------------------------------------------
int ctkHighPrecisionTimerTest(int /*argc*/, char* /*argv*/[])
{
  ctkHighPrecisionTimer timer;
  timer.start();
  QTest::qSleep(250);
  qint64 millis = timer.elapsedMilli();
  qint64 micros = timer.elapsedMicro();
  if (millis < 200 || millis > 300 ||
      micros < 200*1000 || micros > 300*1000)
  {
    qDebug() << "Measured time (" << millis << "ms | " << micros << "us) is not between 200 and 300ms.";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
