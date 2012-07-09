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

#include "ctkDicomExchangeInterface.h"

struct ctkDicomHostInterface : public virtual ctkDicomExchangeInterface
{
  // Host interface methods

  /**
   * Returns a newly created DICOM UID that the Hosted Application might use, e.g., to create new data
   * objects and structures.
   */
  virtual QString generateUID() = 0;

  /**
   * The Hosted Application supplies its preferred screen size in the appPreferredScreen parameter. The
   * Hosting System may utilize this information as a hint, but may return a window location and size that best
   * suits the Hosting System's GUI.
   */
  virtual QRect getAvailableScreen(const QRect& preferredScreen) = 0;

  /**
   * This method returns a URI that a Hosted Application may use to store output that it may provide back to
   * the Hosting System (e.g. in response to a getData() call).
   * \return a URI that a Hosted Application may use to store output.
   */
  virtual QString getOutputLocation(const QStringList& preferredProtocols) = 0;

  /**
   * The Hosted Application shall invoke this method each time the Hosted Application successfully transitions
   * to a new state. The new state is passed in the state parameter.
   */
  virtual void notifyStateChanged(ctkDicomAppHosting::State state) = 0;

  /**
   * Method used by the Hosted Application to inform the Hosting System of notable events that occur during execution.
   * The Hosted Application invoks this method, passing the information in the status parameter.
   */
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status) = 0;

  // Data exchange interface methods
  // inherited from ctkDicomExchangeInterface

};

Q_DECLARE_INTERFACE(ctkDicomHostInterface, "org.commontk.dah.core.HostInterface")

#endif // CTKDICOMHOSTINTERFACE_H
