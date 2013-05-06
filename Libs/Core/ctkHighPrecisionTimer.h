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

#ifndef CTKHIGHPRECISIONTIMER_H
#define CTKHIGHPRECISIONTIMER_H


#include <qglobal.h>

#undef _ctk_use_high_precision_timer_fallback

#ifdef Q_OS_MAC
#include <mach/mach_time.h>
#elif defined(Q_OS_UNIX)
#include <time.h>
#include <unistd.h>
#ifndef _POSIX_MONOTONIC_CLOCK
#warning Monotonic clock support missing on this POSIX platform
#define _ctk_use_high_precision_timer_fallback
#endif
#elif defined(Q_OS_WIN)
#include <windows.h>
#else
#define _ctk_use_high_precision_timer_fallback
#endif

#ifdef _ctk_use_high_precision_timer_fallback
#warning ctkHighPrecisionTimer is using the slower QTime fallback
#include <QTime>
#endif


/**
 * \ingroup Core
 *
 *
 * @brief A fast and high precision timer.
 *
 * This class provides a fast and high precision timer depending on
 * platform specific API. It can be used as a QTime replacement for
 * runtime measurements with a minimal performance overhead.
 */
class ctkHighPrecisionTimer {

public:

  inline ctkHighPrecisionTimer();

  inline void start();

  inline qint64 elapsedMilli();

  inline qint64 elapsedMicro();

private:

  // only used on MacOS, but needs to be defined in the .cpp file
  static double timeConvert;

#ifdef _ctk_use_high_precision_timer_fallback
  QTime startTime;
#elif defined(Q_OS_MAC)
  quint64 startTime;
#elif defined(Q_OS_UNIX)
  timespec startTime;
#elif defined(Q_OS_WIN)
  LARGE_INTEGER timerFrequency;
  LARGE_INTEGER startTime;
#endif
};

#ifdef _ctk_use_high_precision_timer_fallback

inline ctkHighPrecisionTimer::ctkHighPrecisionTimer()
  : startTime(QTime::currentTime())
{
}

inline void ctkHighPrecisionTimer::start()
{
  startTime = QTime::currentTime();
}

inline qint64 ctkHighPrecisionTimer::elapsedMilli()
{
  return startTime.elapsed();
}

inline qint64 ctkHighPrecisionTimer::elapsedMicro()
{
  return startTime.elapsed() * 1000;
}

#elif defined(Q_OS_MAC)

inline ctkHighPrecisionTimer::ctkHighPrecisionTimer()
: startTime(0)
{
  if (timeConvert == 0)
  {
    mach_timebase_info_data_t timeBase;
    mach_timebase_info(&timeBase);
    timeConvert = static_cast<double>(timeBase.numer) / static_cast<double>(timeBase.denom) / 1000.0;
  }
}

inline void ctkHighPrecisionTimer::start()
{
  startTime = mach_absolute_time();
}

inline qint64 ctkHighPrecisionTimer::elapsedMilli()
{
  quint64 current = mach_absolute_time();
  return static_cast<double>(current - startTime) * timeConvert / 1000.0;
}

inline qint64 ctkHighPrecisionTimer::elapsedMicro()
{
  quint64 current = mach_absolute_time();
  return static_cast<double>(current - startTime) * timeConvert;
}

#elif defined(Q_OS_UNIX)

inline ctkHighPrecisionTimer::ctkHighPrecisionTimer()
{
  startTime.tv_nsec = 0;
  startTime.tv_sec = 0;
}

inline void ctkHighPrecisionTimer::start()
{
  clock_gettime(CLOCK_MONOTONIC, &startTime);
}

inline qint64 ctkHighPrecisionTimer::elapsedMilli()
{
  timespec current;
  clock_gettime(CLOCK_MONOTONIC, &current);
  return (static_cast<qint64>(current.tv_sec)*1000 + current.tv_nsec/1000/1000) -
      (static_cast<qint64>(startTime.tv_sec)*1000 + startTime.tv_nsec/1000/1000);
}

inline qint64 ctkHighPrecisionTimer::elapsedMicro()
{
  timespec current;
  clock_gettime(CLOCK_MONOTONIC, &current);
  return (static_cast<qint64>(current.tv_sec)*1000*1000 + current.tv_nsec/1000) -
      (static_cast<qint64>(startTime.tv_sec)*1000*1000 + startTime.tv_nsec/1000);
}

#elif defined(Q_OS_WIN)

#include "ctkException.h"

inline ctkHighPrecisionTimer::ctkHighPrecisionTimer()
{
  if (!QueryPerformanceFrequency(&timerFrequency))
    throw ctkRuntimeException("QueryPerformanceFrequency() failed");
}

inline void ctkHighPrecisionTimer::start()
{
  //DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
  QueryPerformanceCounter(&startTime);
  //SetThreadAffinityMask(GetCurrentThread(), oldmask);
}

inline qint64 ctkHighPrecisionTimer::elapsedMilli()
{
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  return (current.QuadPart - startTime.QuadPart) / (timerFrequency.QuadPart / 1000);
}

inline qint64 ctkHighPrecisionTimer::elapsedMicro()
{
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  return (current.QuadPart - startTime.QuadPart) / (timerFrequency.QuadPart / (1000*1000));
}

#endif

#endif // CTKHIGHPRECISIONTIMER_H
