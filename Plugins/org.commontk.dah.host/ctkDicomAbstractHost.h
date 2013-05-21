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

#ifndef CTKDICOMABSTRACTHOST_H
#define CTKDICOMABSTRACTHOST_H

#include <ctkDicomAbstractExchangeCache.h>
#include <ctkDicomHostInterface.h>
#include <ctkDicomAppInterface.h>
#include <QScopedPointer>

#include <org_commontk_dah_host_Export.h>

class ctkDicomAbstractHostPrivate;
class ctkDicomObjectLocatorCache;

#ifdef _MSC_VER
// disable inheritance by dominance warnings
#pragma warning( disable : 4250 )
#endif

/**
 * @brief Provides a basic implementation for an application host.
 *
 * It starts a http server and serves one hosted application. Multiple instances
 * can be used for hosting multiple applications.
 *
 * The methods of the ctkDicomHostInterface have to be implemented for the business logic,
 *
*/
class org_commontk_dah_host_EXPORT ctkDicomAbstractHost : public ctkDicomAbstractExchangeCache, public ctkDicomHostInterface
{
 Q_OBJECT
 Q_INTERFACES(ctkDicomHostInterface)

public:

  /**
   * @brief Starts the soap server on the specified port or choose port automatically.
   *
   * @param hostPort
   * @param appPort
  */
  ctkDicomAbstractHost(int hostPort = 0, int appPort = 0);

  /**
   * @brief
   *
  */
  virtual ~ctkDicomAbstractHost();

  /**
   * @brief Gets the host port.
   *
   * @return int
  */
  int getHostPort() const;

  /**
   * @brief Gets the hosted application port.
   *
   * @return int
  */
  int getAppPort() const;

  /**
   * @brief Handles transitions form one state to the other.
   * When a new state notification arrives from the hosted app,
   * it goes through a state machine check and triggers signals depending on the appropriate response.
   *
   * The developer must connect to these signals to react to new state. The signal #stateChangedReceived is not to be used for this, it is just there for propagating new states for information only.
   *
   * @see appReady() releaseAvailableResources() startProgress() resumed() completed() suspended() canceled() exited() stateChangedReceived()
   * @param state
  */
  virtual void notifyStateChanged(ctkDicomAppHosting::State state);

  /**
   * @brief Gets the internal representation of the application state.
   * Does not call the client. For that purpose call ctkDicomAppInterface::getState() instead.
   *
   * @return ctkDicomAppHosting::State
  */
  ctkDicomAppHosting::State getApplicationState() const;

  /**
   * @brief Gets the application service in order to call methods on the hosted app.
   *
   * @return ctkDicomAppInterface *
  */
  ctkDicomAppInterface* getDicomAppService() const;

  ctkDicomExchangeInterface* getOtherSideExchangeService() const;

Q_SIGNALS:
  /**
   * @brief Emitted when the ctkDicomAppHosting::IDLE state notification has been received, and the previous state was EXIT, IDLE or CANCELED.
   * @todo: perhaps also send this when completed. Needs discussion.
   *
  */
  void appReady();

  /**
   * @brief Emitted when the ctkDicomAppHosting::IDLE state notification has been received, and the previous state was COMPLETED.
   *
  */
  void releaseAvailableResources();
  /**
   * @brief Emitted when the ctkDicomAppHosting::INPROGRESS state notification has been received, and previous state IDLE.
   *
  */
  void startProgress();

  /**
   * @brief Emitted when the ctkDicomAppHosting::INPROGRESS state notification has been received, and previous state SUSPENDED.
   *
  */
  void resumed();

  /**
   * @brief  Emitted when the ctkDicomAppHosting::COMPLETED state notification has been received.
   *
  */
  void completed();

  /**
   * @brief Emitted when the ctkDicomAppHosting::SUSPENDED state notification has been received.
   *
  */
  void suspended();

  /**
   * @brief Emitted when the ctkDicomAppHosting::CANCELED state notification has been received.
   *
  */
  void canceled();

  /**
   * @brief Emitted when the ctkDicomAppHosting::EXIT state notification has been received.
   *
  */
  void exited();

  /**
   * @brief Emitted after any new state has been received.
   * The event is sent after all the others have been sent through the state machine.
   *
   * @param state
  */
  void stateChangedReceived(ctkDicomAppHosting::State state);

  /**
   * @brief
   *
   * @param status
  */
  void statusReceived(const ctkDicomAppHosting::Status& status);

private:

  Q_DECLARE_PRIVATE(ctkDicomAbstractHost)
  const QScopedPointer<ctkDicomAbstractHostPrivate> d_ptr; /**< TODO */

};

#endif // CTKDICOMABSTRACTHOST_H
