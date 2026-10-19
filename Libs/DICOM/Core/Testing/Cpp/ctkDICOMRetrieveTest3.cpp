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

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QList>
#include <QObject>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMRetrieve.h"

// STD includes
#include <cstdlib>
#include <iostream>

// Checks that ctkDICOMRetrieve hands the received frames over in batches and does
// not accumulate more than framesBatchLimit() of them, which is what bounds the
// memory used while retrieving a series.
int ctkDICOMRetrieveTest3( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  const int batchLimit = 10;
  const int numberOfFrames = 50;

  //
  // Default batch size
  //
  {
    ctkDICOMRetrieve retrieve;
    if (retrieve.framesBatchLimit() != 25)
    {
      std::cerr << "Line " << __LINE__ << " - expected a default framesBatchLimit of 25, got "
                << retrieve.framesBatchLimit() << std::endl;
      return EXIT_FAILURE;
    }
  }

  //
  // Batching enabled: the retriever must never hold more than batchLimit frames
  //
  {
    ctkDICOMRetrieve retrieve;
    retrieve.setJobUID("jobUID");
    retrieve.setFramesBatchLimit(batchLimit);

    if (retrieve.framesBatchLimit() != batchLimit)
    {
      std::cerr << "Line " << __LINE__ << " - framesBatchLimit failed: "
                << retrieve.framesBatchLimit() << std::endl;
      return EXIT_FAILURE;
    }

    int numberOfBatches = 0;
    int totalFramesBatched = 0;
    QList<int> batchSizes;

    // Stands in for ctkDICOMRetrieveWorker::onFramesBatchReady: the owner inserts
    // the batch and drops it from the retriever, which releases the frames.
    QObject::connect(&retrieve, &ctkDICOMRetrieve::framesBatchReady,
                     &retrieve,
                     [&](const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets)
    {
      numberOfBatches++;
      batchSizes.append(jobResponseSets.count());
      totalFramesBatched += jobResponseSets.count();
      const QList<QSharedPointer<ctkDICOMJobResponseSet>> batch = jobResponseSets;
      retrieve.removeJobResponseSets(batch);
    }, Qt::DirectConnection);

    int maximumHeldFrames = 0;
    for (int frame = 0; frame < numberOfFrames; ++frame)
    {
      QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
        QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
      jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
      jobResponseSet->setJobUID("jobUID");
      retrieve.addJobResponseSet(jobResponseSet);

      maximumHeldFrames = qMax(maximumHeldFrames, retrieve.jobResponseSetsShared().count());
    }

    if (maximumHeldFrames > batchLimit)
    {
      std::cerr << "Line " << __LINE__ << " - the retriever held " << maximumHeldFrames
                << " frames at once, more than the batch limit of " << batchLimit
                << ": the memory usage is not bounded." << std::endl;
      return EXIT_FAILURE;
    }

    if (numberOfBatches != numberOfFrames / batchLimit)
    {
      std::cerr << "Line " << __LINE__ << " - expected " << numberOfFrames / batchLimit
                << " batches, got " << numberOfBatches << std::endl;
      return EXIT_FAILURE;
    }

    foreach (int batchSize, batchSizes)
    {
      if (batchSize != batchLimit)
      {
        std::cerr << "Line " << __LINE__ << " - expected batches of " << batchLimit
                  << " frames, got one of " << batchSize << std::endl;
        return EXIT_FAILURE;
      }
    }

    if (totalFramesBatched != numberOfFrames)
    {
      std::cerr << "Line " << __LINE__ << " - " << totalFramesBatched << " frames were batched, "
                << "expected " << numberOfFrames << ": some frames would never be inserted."
                << std::endl;
      return EXIT_FAILURE;
    }

    // Everything was handed over and dropped, nothing is left pending
    if (retrieve.jobResponseSetsShared().count() != 0)
    {
      std::cerr << "Line " << __LINE__ << " - " << retrieve.jobResponseSetsShared().count()
                << " frames are still held by the retriever" << std::endl;
      return EXIT_FAILURE;
    }
  }

  //
  // A number of frames that is not a multiple of the batch: the remainder stays
  // pending, to be inserted by the final insert at the end of the operation
  //
  {
    const int numberOfFramesWithRemainder = 55;

    ctkDICOMRetrieve retrieve;
    retrieve.setJobUID("jobUID");
    retrieve.setFramesBatchLimit(batchLimit);

    int totalFramesBatched = 0;
    QObject::connect(&retrieve, &ctkDICOMRetrieve::framesBatchReady,
                     &retrieve,
                     [&](const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets)
    {
      totalFramesBatched += jobResponseSets.count();
      const QList<QSharedPointer<ctkDICOMJobResponseSet>> batch = jobResponseSets;
      retrieve.removeJobResponseSets(batch);
    }, Qt::DirectConnection);

    for (int frame = 0; frame < numberOfFramesWithRemainder; ++frame)
    {
      QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
        QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
      jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
      jobResponseSet->setJobUID("jobUID");
      retrieve.addJobResponseSet(jobResponseSet);
    }

    const int expectedRemainder = numberOfFramesWithRemainder % batchLimit;
    if (retrieve.jobResponseSetsShared().count() != expectedRemainder)
    {
      std::cerr << "Line " << __LINE__ << " - expected " << expectedRemainder
                << " frames left pending, got " << retrieve.jobResponseSetsShared().count()
                << std::endl;
      return EXIT_FAILURE;
    }

    // No frame may be lost: the batched ones plus the pending ones are all of them
    if (totalFramesBatched + retrieve.jobResponseSetsShared().count() != numberOfFramesWithRemainder)
    {
      std::cerr << "Line " << __LINE__ << " - " << totalFramesBatched << " frames batched and "
                << retrieve.jobResponseSetsShared().count() << " pending do not add up to "
                << numberOfFramesWithRemainder << std::endl;
      return EXIT_FAILURE;
    }
  }

  //
  // Batching disabled: every frame is kept until the end of the operation
  //
  {
    ctkDICOMRetrieve retrieve;
    retrieve.setJobUID("jobUID");
    retrieve.setFramesBatchLimit(0);

    int numberOfBatches = 0;
    QObject::connect(&retrieve, &ctkDICOMRetrieve::framesBatchReady,
                     &retrieve,
                     [&](const QList<QSharedPointer<ctkDICOMJobResponseSet>>&)
    {
      numberOfBatches++;
    }, Qt::DirectConnection);

    for (int frame = 0; frame < numberOfFrames; ++frame)
    {
      QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
        QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
      jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
      jobResponseSet->setJobUID("jobUID");
      retrieve.addJobResponseSet(jobResponseSet);
    }

    if (numberOfBatches != 0)
    {
      std::cerr << "Line " << __LINE__ << " - framesBatchReady was emitted " << numberOfBatches
                << " times while batching is disabled" << std::endl;
      return EXIT_FAILURE;
    }

    if (retrieve.jobResponseSetsShared().count() != numberOfFrames)
    {
      std::cerr << "Line " << __LINE__ << " - expected " << numberOfFrames
                << " frames to be held, got " << retrieve.jobResponseSetsShared().count()
                << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cerr << "ctkDICOMRetrieveTest3 passed" << std::endl;
  return EXIT_SUCCESS;
}
