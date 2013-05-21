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


#ifndef CTKDICOMAPPINTERFACE_H
#define CTKDICOMAPPINTERFACE_H

#include "ctkDicomExchangeInterface.h"
#include <ctkDicomAppHostingTypesHelper.h>

struct ctkDicomAppInterface : public virtual ctkDicomExchangeInterface
{

  // Application interface methods
  virtual ctkDicomAppHosting::State getState() = 0;
  
  /**
   * Method triggered by the host. Changes the state of the hosted application.
   * \return true if state received and not illegal in the transition diagram from the reference, false if illegal or not recognized.
   */
  virtual bool setState(ctkDicomAppHosting::State newState) = 0;

  /**
   * Method triggered by the host. By calling this method, the Hosting System is asking the Hosted Application to take whatever steps are
   * needed to make its GUI visible as the topmost window, and to gain focus.
   * \return TRUE if the Hosted Application received the request and will act on it. Otherwise it returns FALSE
   */
  virtual bool bringToFront(const QRect& requestedScreenArea) = 0;

  // Data exchange interface methods
  // inherited from ctkDicomExchangeInterface

};

Q_DECLARE_INTERFACE(ctkDicomAppInterface, "org.commontk.dah.core.AppInterface")

#endif // CTKDICOMAPPINTERFACE_H
