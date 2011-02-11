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


#ifndef CTKEXAMPLEDICOMAPPLOGIC_P_H
#define CTKEXAMPLEDICOMAPPLOGIC_P_H

// Qt includes
#include <QUuid>

// CTK includes
#include <ctkDicomAbstractApp.h>
#include <ctkDicomHostInterface.h>

#include <ctkServiceTracker.h>

struct ctkDicomHostInterface;

class QPushButton;

class ctkExampleDicomAppLogic : public ctkDicomAbstractApp
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomAppInterface)

public:

  ctkExampleDicomAppLogic();
  virtual ~ctkExampleDicomAppLogic();

  // ctkDicomAppInterface
  virtual bool bringToFront(const QRect& requestedScreenArea);

  // ctkDicomExchangeInterface
  virtual bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  virtual void releaseData(const QList<QUuid>& objectUUIDs);

  // some logic
  void do_something();


protected slots:

  void onStartProgress();
  void onResumeProgress();
  void onSuspendProgress();
  void onCancelProgress();
  void onExitHostedApp();
  void onReleaseResources();

  void buttonClicked();
private:
  QPushButton * Button;

  QUuid uuid;

}; // ctkExampleDicomAppLogic

#endif // ctkExampleDicomAppLogic_P_H
