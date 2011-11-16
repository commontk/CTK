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


#ifndef CTKCMSERIALIZEDTASKQUEUE_P_H
#define CTKCMSERIALIZEDTASKQUEUE_P_H

#include <QList>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class QRunnable;

/**
 * ctkCMSerializedTaskQueue is a utility class that will allow asynchronous but serialized execution of tasks
 */
class ctkCMSerializedTaskQueue : public QObject
{
  Q_OBJECT

public:

  ctkCMSerializedTaskQueue(const QString& queueName);
  ~ctkCMSerializedTaskQueue();

  void put(QRunnable* newTask);

protected Q_SLOTS:

  void runTasks();

protected:

  QRunnable* nextTask(int maxWait);

private:

  static const int MAX_WAIT; // = 5000
  QList<QRunnable*> tasks;
  QThread thread;
  QMutex mutex;
  QWaitCondition waitForTask;
};

#endif // CTKCMSERIALIZEDTASKQUEUE_P_H
