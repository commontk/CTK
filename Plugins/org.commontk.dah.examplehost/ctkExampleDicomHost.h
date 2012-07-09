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

#ifndef CTKEXAMPLEDICOMHOST_H
#define CTKEXAMPLEDICOMHOST_H

// Qt includes
#include <QUrl>
#include <QProcess>

// CTK includes
#include <ctkDicomAbstractHost.h>
#include <ctkHostedAppPlaceholderWidget.h>

#include <org_commontk_dah_examplehost_Export.h>


class org_commontk_dah_examplehost_EXPORT ctkExampleDicomHost :  public ctkDicomAbstractHost
{
  Q_OBJECT

public:

  ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort = 8080, int appPort = 8081);
  virtual ~ctkExampleDicomHost();

  virtual void StartApplication(QString AppPath);

  /**
   * Returns a newly created DICOM UID that the Hosted Application might use, e.g., to create new data
   * objects and structures.
   */
  virtual QString generateUID();

  /**
   * The Hosted Application supplies its preferred screen size in the appPreferredScreen parameter. The
   * Hosting System may utilize this information as a hint, but may return a window location and size that best
   * suits the Hosting System's GUI.
   */
  virtual QRect getAvailableScreen(const QRect& preferredScreen);
  
  /**
   * This method returns a URI that a Hosted Application may use to store output that it may provide back to
   * the Hosting System (e.g. in response to a getData() call).
   * \return a URI that a Hosted Application may use to store output.
   */
  virtual QString getOutputLocation(const QStringList& preferredProtocols);

  /**
   * Method used by the Hosted Application to inform the Hosting System of notable events that occur during execution.
   * The Hosted Application invoks this method, passing the information in the status parameter.
   */
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status);

  // exchange methods
  /**
   * The source of the data calls this method with descriptions of the available data that it can provide to the
   * recipient. If the source of the data expects that additional data will become available, it shall pass FALSE
   * in the lastData parameter. Otherwise, it shall pass TRUE.
   * \return TRUE if the recipient of the data successfully received the AvailableData list.
   */
  virtual bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  ctkDicomAppHosting::State getApplicationState() const;

  const QProcess& getAppProcess() const { return this->AppProcess; }
  void exitApplication();

  QByteArray processReadAll(){return this->AppProcess.readAllStandardOutput ();}

public slots:
  void onAppReady();
  void onReleaseAvailableResources();
  void onStartProgress();
  void onResumed();
  void onCompleted();
  void onSuspended();
  void onCanceled();
  void onExited();

signals:


  void giveAvailableScreen(QRect rect);

protected:

  QProcess AppProcess;
  ctkHostedAppPlaceholderWidget* PlaceholderWidget;
private:
  bool exitingApplication;
};

#endif // CTKEXAMPLEDICOMHOST_H
