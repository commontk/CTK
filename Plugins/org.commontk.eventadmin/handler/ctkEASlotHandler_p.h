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


#ifndef CTKEASLOTHANDLER_P_H
#define CTKEASLOTHANDLER_P_H

#include <QObject>

#include <ctkServiceRegistration.h>
#include <service/event/ctkEventHandler.h>

class ctkEASlotHandler : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

public:

  ctkServiceRegistration reg;

  void updateProperties(const ctkDictionary& properties);

  void handleEvent(const ctkEvent& event);

Q_SIGNALS:

  void eventOccured(const ctkEvent& event);

};

#endif // CTKEASLOTHANDLER_P_H
