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


#include "ctkEAThreadFactoryUser_p.h"

#include <QRunnable>

ctkEAThreadFactoryUser::DefaultThread::DefaultThread(ctkEARunnable* command)
  : command(command)
{ }

void ctkEAThreadFactoryUser::DefaultThread::run()
{
  const bool autoDelete = command->autoDelete();
  command->run();
  if (autoDelete && !--command->ref) delete command;
}


ctkEAInterruptibleThread* ctkEAThreadFactoryUser::DefaultThreadFactory::newThread(ctkEARunnable* command)
{
  return new DefaultThread(command);
}


ctkEAThreadFactoryUser::ctkEAThreadFactoryUser()
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  : mutex(),
#else
  : mutex(QMutex::Recursive),
#endif
    threadFactory(new DefaultThreadFactory())
{

}

ctkEAThreadFactoryUser::~ctkEAThreadFactoryUser()
{
  delete threadFactory;
}

ctkEAThreadFactory* ctkEAThreadFactoryUser::setThreadFactory(ctkEAThreadFactory* factory)
{
  QMutexLocker lock(&mutex);
  ctkEAThreadFactory* old = threadFactory;
  threadFactory = factory;
  return old;
}

ctkEAThreadFactory* ctkEAThreadFactoryUser::getThreadFactory()
{
  return threadFactory;
}
