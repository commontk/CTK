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


#ifndef CTKDICOMHOSTINTERFACE_H
#define CTKDICOMHOSTINTERFACE_H

#include <QObject>
#include <QRect>

#include "ctkDicomAppHostingTypes.h"
#include "ctkDicomExchangeInterface.h"

#include <org_commontk_dah_core_Export.h>

class org_commontk_dah_core_EXPORT ctkDicomHostInterface: public ctkDicomExchangeInterface
{
  Q_OBJECT

public:

  // Host interface methods
  virtual QString generateUID() = 0;
  virtual QRect getAvailableScreen(const QRect& preferredScreen) = 0;
  virtual QString getOutputLocation(const QStringList& preferredProtocols) = 0;
  virtual void notifyStateChanged(ctkDicomAppHosting::State state) = 0;
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status) = 0;

  // Data exchange interface methods
  // inherited from ctkDicomExchangeInterface

};

#endif // CTKDICOMHOSTINTERFACE_H
