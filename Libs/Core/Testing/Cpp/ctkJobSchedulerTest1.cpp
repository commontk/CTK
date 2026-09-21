/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QAtomicInt>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>

// CTK includes
#include "ctkAbstractJob.h"
#include "ctkAbstractWorker.h"
#include "ctkCoreTestingMacros.h"
#include "ctkJobScheduler.h"

// STD includes
#include <functional>
#include <iostream>

namespace
{

/// Number of jobs of each group currently running, and the maximum reached.
/// A job of the test blocks until Release is set, so these tell how many jobs the
/// scheduler let run at the same time for one group.
QAtomicInt RunningJobsOfGroupA(0);
QAtomicInt RunningJobsOfGroupB(0);
QAtomicInt MaximumRunningJobsOfGroupA(0);
QAtomicInt MaximumRunningJobsOfGroupB(0);
QAtomicInt Release(0);

/// Number of attempts run by the failing job of the retry test
QAtomicInt FailingJobAttempts(0);

//------------------------------------------------------------------------------
void resetCounters()
{
  FailingJobAttempts.storeRelaxed(0);
  RunningJobsOfGroupA.storeRelaxed(0);
  RunningJobsOfGroupB.storeRelaxed(0);
  MaximumRunningJobsOfGroupA.storeRelaxed(0);
  MaximumRunningJobsOfGroupB.storeRelaxed(0);
  Release.storeRelaxed(0);
}

//------------------------------------------------------------------------------
class ctkTestJob : public ctkAbstractJob
{
  Q_OBJECT

public:
  explicit ctkTestJob(const QString& group = QString(), QObject* parent = nullptr)
    : ctkAbstractJob(parent)
    , Group(group)
  {
  }

  /// If true, the worker reports a failure instead of running, so that the
  /// scheduler reattempts the job.
  bool AlwaysFails{false};

  QString concurrencyGroup() const override { return this->Group; }

  ctkAbstractWorker* createWorker() override;

  ctkAbstractJob* clone() const override
  {
    ctkTestJob* newJob = new ctkTestJob(this->Group);
    newJob->AlwaysFails = this->AlwaysFails;
    newJob->setRetryEnabled(this->retryEnabled());
    newJob->setRetryDelay(this->retryDelay());
    newJob->setRetryBackoffFactor(this->retryBackoffFactor());
    newJob->setMaximumRetryWait(this->maximumRetryWait());
    newJob->setRetryCounter(this->retryCounter());
    newJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
    newJob->setMaximumConcurrentJobsPerGroup(this->maximumConcurrentJobsPerGroup());
    newJob->setPriority(this->priority());
    return newJob;
  }

  QString loggerReport(const QString& status) override { return status; }
  void releaseResources() override {}

  QString Group;
};

//------------------------------------------------------------------------------
class ctkTestWorker : public ctkAbstractWorker
{
  Q_OBJECT

public:
  void requestCancel() override {}

  void run() override
  {
    QSharedPointer<ctkTestJob> job = qSharedPointerObjectCast<ctkTestJob>(this->Job);
    if (!job)
    {
      return;
    }

    if (job->AlwaysFails)
    {
      FailingJobAttempts.fetchAndAddOrdered(1);
      // false: the job was not canceled by the user, it failed on its own
      this->onJobCanceled(false);
      return;
    }

    QAtomicInt& running = job->Group == "A" ? RunningJobsOfGroupA : RunningJobsOfGroupB;
    QAtomicInt& maximum = job->Group == "A" ? MaximumRunningJobsOfGroupA : MaximumRunningJobsOfGroupB;

    job->setStatus(ctkAbstractJob::JobStatus::Running);

    int current = running.fetchAndAddOrdered(1) + 1;
    int previousMaximum = maximum.loadAcquire();
    while (current > previousMaximum &&
           !maximum.testAndSetOrdered(previousMaximum, current))
    {
      previousMaximum = maximum.loadAcquire();
    }

    // Hold the job running until the test has looked at how many jobs run together
    QElapsedTimer timer;
    timer.start();
    while (Release.loadAcquire() == 0 && timer.elapsed() < 10000)
    {
      QThread::msleep(5);
    }

    running.fetchAndSubOrdered(1);
    job->setStatus(ctkAbstractJob::JobStatus::Finished);
  }
};

//------------------------------------------------------------------------------
ctkAbstractWorker* ctkTestJob::createWorker()
{
  ctkTestWorker* worker = new ctkTestWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
/// Process events until the condition is met or the timeout elapses.
bool waitFor(const std::function<bool()>& condition, int timeoutMsec = 10000)
{
  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < timeoutMsec)
  {
    if (condition())
    {
      return true;
    }
    QCoreApplication::processEvents();
    QThread::msleep(5);
  }
  return condition();
}

} // end of anonymous namespace

//------------------------------------------------------------------------------
int ctkJobSchedulerTest1(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  // Default values
  ctkJobScheduler scheduler;
  CHECK_INT(scheduler.maximumThreadCount(), 20);
  CHECK_INT(scheduler.retryDelay(), 1000);
  CHECK_INT(scheduler.maximumRetryWait(), 60000);

  scheduler.setRetryDelay(50);
  CHECK_INT(scheduler.retryDelay(), 50);
  scheduler.setMaximumRetryWait(500);
  CHECK_INT(scheduler.maximumRetryWait(), 500);

  //
  // Retry policy: the delay grows exponentially with the attempts, is randomized,
  // and the sum of the delays never exceeds the retry window.
  //
  ctkTestJob retryJob;
  retryJob.setRetryDelay(100);
  retryJob.setRetryBackoffFactor(2.);
  retryJob.setMaximumRetryWait(1000000);

  int previousDelay = 0;
  for (int attempt = 0; attempt < 5; ++attempt)
  {
    retryJob.setRetryCounter(attempt);
    const int delayWithoutJitter = 100 * (1 << attempt);
    const int delay = retryJob.nextRetryDelay();
    // The delay is the exponential one, give or take the 25% of jitter
    CHECK_BOOL(delay >= (delayWithoutJitter * 3) / 4, true);
    CHECK_BOOL(delay <= (delayWithoutJitter * 5) / 4, true);
    // Even with the jitter, an attempt always waits longer than the previous one
    CHECK_BOOL(delay > previousDelay, true);
    previousDelay = delay;
  }

  // The delay is randomized, so that the jobs of a server that went down do not all
  // come back to it at the same time
  retryJob.setRetryCounter(4);
  bool delaysDiffer = false;
  const int firstDelay = retryJob.nextRetryDelay();
  for (int sample = 0; sample < 20 && !delaysDiffer; ++sample)
  {
    delaysDiffer = retryJob.nextRetryDelay() != firstDelay;
  }
  CHECK_BOOL(delaysDiffer, true);

  // A delay that would overshoot the retry window is cut down to what is left of it,
  // so that a last attempt is made exactly at the end of the window
  retryJob.setMaximumRetryWait(5000);
  retryJob.setAccumulatedRetryWait(4200);
  retryJob.setRetryCounter(10);
  CHECK_INT(retryJob.nextRetryDelay(), 800);

  // Once the window is spent, the job is not reattempted
  retryJob.setAccumulatedRetryWait(5000);
  CHECK_INT(retryJob.nextRetryDelay(), -1);

  // and the GUI can tell this failure from a job that simply failed once
  ctkJobDetail retryTimeoutDetail(retryJob);
  CHECK_BOOL(retryTimeoutDetail.retryWaitElapsed(), true);
  retryJob.setRetryCounter(0);
  retryJob.setAccumulatedRetryWait(0);
  ctkJobDetail firstFailureDetail(retryJob);
  CHECK_BOOL(firstFailureDetail.retryWaitElapsed(), false);

  // Retrying can also be disabled outright, or by an empty window
  retryJob.setRetryEnabled(false);
  CHECK_INT(retryJob.nextRetryDelay(), -1);
  retryJob.setRetryEnabled(true);
  retryJob.setMaximumRetryWait(0);
  CHECK_INT(retryJob.nextRetryDelay(), -1);

  //
  // Maximum concurrent jobs per group: jobs of one group do not take over the pool,
  // and they do not hold back the jobs of the other groups either.
  //
  resetCounters();

  const int numberOfJobsPerGroup = 5;
  const int maximumConcurrentJobsOfGroupA = 2;
  for (int i = 0; i < numberOfJobsPerGroup; ++i)
  {
    ctkTestJob* jobOfGroupA = new ctkTestJob("A");
    jobOfGroupA->setRetryEnabled(false);
    jobOfGroupA->setMaximumConcurrentJobsPerGroup(maximumConcurrentJobsOfGroupA);
    scheduler.addJob(jobOfGroupA);

    ctkTestJob* jobOfGroupB = new ctkTestJob("B");
    jobOfGroupB->setRetryEnabled(false);
    // No limit for this group
    jobOfGroupB->setMaximumConcurrentJobsPerGroup(0);
    scheduler.addJob(jobOfGroupB);
  }

  CHECK_INT(scheduler.numberOfJobs(), 2 * numberOfJobsPerGroup);

  // Every job of the unlimited group runs, whatever the other group does
  CHECK_BOOL(waitFor([numberOfJobsPerGroup]()
    {
      return RunningJobsOfGroupB.loadAcquire() == numberOfJobsPerGroup;
    }), true);

  // The limited group never has more than its share running
  CHECK_BOOL(waitFor([maximumConcurrentJobsOfGroupA]()
    {
      return RunningJobsOfGroupA.loadAcquire() == maximumConcurrentJobsOfGroupA;
    }), true);
  CHECK_INT(MaximumRunningJobsOfGroupA.loadAcquire(), maximumConcurrentJobsOfGroupA);
  CHECK_INT(MaximumRunningJobsOfGroupB.loadAcquire(), numberOfJobsPerGroup);

  // Let the jobs finish: the jobs of the limited group that were held back run now,
  // still never more than the limit at a time.
  Release.storeRelease(1);

  CHECK_BOOL(waitFor([&scheduler]()
    {
      return scheduler.numberOfRunningJobs() == 0;
    }), true);
  CHECK_INT(MaximumRunningJobsOfGroupA.loadAcquire(), maximumConcurrentJobsOfGroupA);

  scheduler.stopAllJobs(true);

  //
  // A job that keeps failing is reattempted after growing delays, and gives up once
  // its retry window is spent.
  //
  resetCounters();
  Release.storeRelease(1); // the retried job must not block

  ctkTestJob* failingJob = new ctkTestJob("A");
  failingJob->AlwaysFails = true;
  failingJob->setRetryDelay(20);
  failingJob->setRetryBackoffFactor(2.);
  failingJob->setMaximumRetryWait(200);
  scheduler.addJob(failingJob);

  // 20 + 40 + 80 + 60 (what is left of the 200 ms window) = 4 delays, so 5 attempts
  CHECK_BOOL(waitFor([]()
    {
      return FailingJobAttempts.loadAcquire() >= 5;
    }), true);

  // and then it stops being reattempted
  const int attemptsWhenWindowElapsed = FailingJobAttempts.loadAcquire();
  waitFor([]() { return false; }, 300);
  CHECK_INT(FailingJobAttempts.loadAcquire(), attemptsWhenWindowElapsed);

  scheduler.stopAllJobs(true);

  //
  // Stopping the jobs also drops the ones waiting for their next attempt.
  //
  resetCounters();
  Release.storeRelease(1);

  ctkTestJob* stoppedFailingJob = new ctkTestJob("A");
  stoppedFailingJob->AlwaysFails = true;
  stoppedFailingJob->setRetryDelay(400);
  stoppedFailingJob->setMaximumRetryWait(60000);
  scheduler.addJob(stoppedFailingJob);

  CHECK_BOOL(waitFor([]()
    {
      return FailingJobAttempts.loadAcquire() == 1;
    }), true);

  scheduler.stopAllJobs(true);

  // The delay of the second attempt elapses, but that attempt never runs
  waitFor([]() { return false; }, 800);
  CHECK_INT(FailingJobAttempts.loadAcquire(), 1);

  return EXIT_SUCCESS;
}

#include "ctkJobSchedulerTest1.moc"
