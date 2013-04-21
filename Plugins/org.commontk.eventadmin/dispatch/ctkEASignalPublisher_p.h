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


#ifndef CTKEASIGNALPUBLISHER_P_H
#define CTKEASIGNALPUBLISHER_P_H

#include <QObject>

#include <service/event/ctkEvent.h>

class ctkEventAdminService;

class ctkEASignalPublisher : public QObject
{
  Q_OBJECT

public:
  ctkEASignalPublisher(ctkEventAdminService* eventAdmin,
                       const QString& signal, const QString& topic);

  QString getSignalName() const;
  QString getTopicName() const;

public slots:

  void publishSyncSignal(const ctkDictionary& eventProps);
  void publishAsyncSignal(const ctkDictionary& eventProps);

private:

  ctkEventAdminService* eventAdmin;
  const QString signal;
  const QString topic;
};

#endif // CTKEASIGNALPUBLISHER_P_H
