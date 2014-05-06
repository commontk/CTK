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

#ifndef CTKDICOMABSTRACTAPP_H
#define CTKDICOMABSTRACTAPP_H

#include <ctkDicomAbstractExchangeCache.h>
#include <ctkDicomAppInterface.h>
#include <QScopedPointer>
#include <org_commontk_dah_hostedapp_Export.h>

class ctkDicomAbstractAppPrivate;
struct ctkDicomHostInterface;
class ctkPluginContext;
class ctkDicomObjectLocatorCache;

#ifdef _MSC_VER
// disable inheritance by dominance warnings
#pragma warning( disable : 4250 )
#endif

/**
 * @brief Provides a basic implementation for an application app.
 *
 * The methods of the ctkDicomAppInterface have to be implemented for the business logic.
 *
 * @todo Provide helper/convenience methods to ease application development.
 *
 *
*/
class org_commontk_dah_hostedapp_EXPORT ctkDicomAbstractApp : public ctkDicomAbstractExchangeCache, public ctkDicomAppInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomAppInterface)

public:

  /**
   * @brief
   *
   * @param context
  */
  ctkDicomAbstractApp(ctkPluginContext* context);

  /**
   * @brief
   *
  */
  virtual ~ctkDicomAbstractApp();


  /**
   * @brief Method triggered by the host. Changes the state of the hosted application.
   *
   * Goes through the transitions and emits signals when relevant to trigger actions. Checks for the legality of a transition.
   *
   * @see startProgress() resumeProgress() suspendProgress() cancelProgress() exitHostedApp() releaseResources()
   * @param newState
   * @return bool true if state received and not illegal in the transition diagram from the reference, false if illegal or not recognized.
  */
  virtual bool setState(ctkDicomAppHosting::State newState);

  /**
   * @brief Sends the current state the app is in to the hosting system.
   *
   * @return ctkDicomAppHosting::State
  */
  virtual ctkDicomAppHosting::State getState();

  /**
   * @brief Gets a handle to the host, in order to call methods on it.
   *
   * @return ctkDicomHostInterface *
  */
  virtual ctkDicomHostInterface* getHostInterface() const;

  ctkDicomExchangeInterface* getOtherSideExchangeService() const;

  /**
   * @brief Sets the internal representation of the current state.
   *
   * @param state
  */
  void setInternalState(ctkDicomAppHosting::State state);

Q_SIGNALS:
  /**
   * @brief ctkDicomAppHosting::INPROGRESS state received and legal.
   *
   * the slot connected to this is responsible for notifying the hosting service that it is really in progress by a call to getHostInterface()->notifyStateChanged(ctkDicomAppHosting::INPROGRESS);
   *
  */
  void startProgress();

  /**
   * @brief ctkDicomAppHosting::INPROGRESS state received when the app is in the ctkDicomAppHosting::SUSPENDED state.
   *
   * Corresponding slot responsible for calling getHostInterface()->notifyStateChanged(ctkDicomAppHosting::INPROGRESS); when resumed.
   *
  */
  void resumeProgress();

  /**
   * @brief ctkDicomAppHosting::SUSPENDED state received.
   *
   * Corresponding slot responsible for calling getHostInterface()->notifyStateChanged(ctkDicomAppHosting::SUSPENDED); when resources have been released.
   *
  */
  void suspendProgress();

  /**
   * @brief ctkDicomAppHosting::CANCELED state received.
   *
   * The CANCELED state is particular because it leads to ctkDicomAppHosting::IDLE when the app has canceled the process.
   * Therefore the notification the app entered this CANCELED state is sent straight away by the setState method.
   *
   * The slot connected to this signal MUST NOT notify CANCELED, but MUST notify IDLE when the resources have been released.
   *
  */
  void cancelProgress();

  /**
   * @brief ctkDicomAppHosting::EXIT state received and legal.
   *
   * An EXIT notification is sent by setState, no communication with the hosting system is needed anymore.
   * A slot connected to this should stop the application process for good.
   *
  */
  void exitHostedApp();

  /**
   * @brief ctkDicomAppHosting::IDLE state received and legal.
   *
   * It means the current state was COMPLETED, and it is now time to release the resources, the hosting system doesn't need them any more.
   *
  */
  void releaseResources();

private:
  Q_DECLARE_PRIVATE(ctkDicomAbstractApp)
  const QScopedPointer<ctkDicomAbstractAppPrivate> d_ptr; /**< TODO */
};

#endif // CTKDICOMABSTRACTAPP_H
