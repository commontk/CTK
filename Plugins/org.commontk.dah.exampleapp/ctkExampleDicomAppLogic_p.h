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

// ui of this application
#include "ui_ctkExampleDicomAppWidget.h"

struct ctkDicomHostInterface;

class QWidget;

class ctkExampleDicomAppLogic : public ctkDicomAbstractApp
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomAppInterface)

public:

  ctkExampleDicomAppLogic();
  virtual ~ctkExampleDicomAppLogic();

  // ctkDicomAppInterface

  /**
   * Method triggered by the host. By calling this method, the Hosting System is asking the Hosted Application to take whatever steps are
   * needed to make its GUI visible as the topmost window, and to gain focus.
   * \return TRUE if the Hosted Application received the request and will act on it. Otherwise it returns FALSE
   */
  virtual bool bringToFront(const QRect& requestedScreenArea);

  // ctkDicomExchangeInterface

  /**
   * The source of the data calls this method with descriptions of the available data that it can provide to the
   * recipient. If the source of the data expects that additional data will become available, it shall pass FALSE
   * in the lastData parameter. Otherwise, it shall pass TRUE.
   * \return TRUE if the recipient of the data successfully received the AvailableData list.
   */
  virtual bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  /**
   * The recipient of data invokes this method to release access to binary data provided by the source of the
   * data through a getData() call. The ArrayOfUUID identifies the data streams that the recipient is releasing.
   */
  virtual void releaseData(const QList<QUuid>& objectUUIDs);

  // some logic
  /** Test function for checking */
  void do_something();


protected slots:

  void onStartProgress();
  void onResumeProgress();
  void onSuspendProgress();
  void onCancelProgress();
  void onExitHostedApp();
  void onReleaseResources();

  void onLoadDataClicked();
  void onCreateSecondaryCapture();
private:
  QWidget * AppWidget;
  Ui::ctkExampleDicomAppWidget ui;

  QUuid uuid;

}; // ctkExampleDicomAppLogic

#endif // ctkExampleDicomAppLogic_P_H
