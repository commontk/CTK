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


#ifndef CTKDICOMHOSTSERVICE_P_H
#define CTKDICOMHOSTSERVICE_P_H

// CTK includes
#include <ctkDicomHostInterface.h>
#include <ctkDicomExchangeService.h>

class ctkDicomHostService : public ctkDicomExchangeService, public ctkDicomHostInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomHostInterface)

public:
  ctkDicomHostService(ushort port, QString path);
  virtual ~ctkDicomHostService();

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
   * The Hosted Application shall invoke this method each time the Hosted Application successfully transitions
   * to a new state. The new state is passed in the state parameter.
   */
  virtual void notifyStateChanged(ctkDicomAppHosting::State state);

  /**
   * Method used by the Hosted Application to inform the Hosting System of notable events that occur during execution.
   * The Hosted Application invoks this method, passing the information in the status parameter.
   */
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status);

  // Exchange methods implemented in ctkDicomExchangeService
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

};

#endif // CTKDICOMHOSTSERVICE_P_H
