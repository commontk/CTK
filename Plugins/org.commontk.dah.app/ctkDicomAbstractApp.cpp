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

// CTK includes
#include "ctkDicomAbstractApp.h"
#include <ctkDicomHostInterface.h>
#include <ctkDicomObjectLocatorCache.h>
#include <ctkPluginContext.h>
#include <ctkServiceTracker.h>

class ctkDicomAbstractAppPrivate
{
public:
  ctkDicomAbstractAppPrivate(ctkPluginContext* context);

  ctkServiceTracker<ctkDicomHostInterface*> HostTracker;
  ctkDicomAppHosting::State currentState;

  ctkDicomObjectLocatorCache ObjectLocatorCache;
};

//----------------------------------------------------------------------------
// ctkDicomAbstractAppPrivate methods

//----------------------------------------------------------------------------
ctkDicomAbstractAppPrivate::ctkDicomAbstractAppPrivate(ctkPluginContext * context):HostTracker(context),currentState(ctkDicomAppHosting::IDLE)
{
  //perhaps notStarted or some dummy state instead of IDLE?
}

//----------------------------------------------------------------------------
// ctkDicomAbstractApp methods

//----------------------------------------------------------------------------
ctkDicomAbstractApp::ctkDicomAbstractApp(ctkPluginContext* context) : d_ptr(new ctkDicomAbstractAppPrivate(context))
{
  d_ptr->HostTracker.open();
}

//----------------------------------------------------------------------------
ctkDicomAbstractApp::~ctkDicomAbstractApp()
{
}

//----------------------------------------------------------------------------
bool ctkDicomAbstractApp::setState(ctkDicomAppHosting::State newState)
{
  bool result = true;
  //received a new state,
  switch (newState){
  case ctkDicomAppHosting::IDLE:
    if (d_ptr->currentState == ctkDicomAppHosting::COMPLETED)
    {

    }
    else
    {
      result = false;
    }
    break;
  case ctkDicomAppHosting::INPROGRESS:
    if (d_ptr->currentState == ctkDicomAppHosting::IDLE)
    {
      emit startProgress();
    }
    else if(d_ptr->currentState == ctkDicomAppHosting::SUSPENDED)
    {
      emit resumeProgress();
    }
    else
    {
      result = false;
    }
    break;
  case ctkDicomAppHosting::COMPLETED:
    qDebug() << "Hosting system shouldn't send completed";
    result = false;
    break;
  case ctkDicomAppHosting::SUSPENDED:
    //suspend computation, release as much resource as possible with possible resuming of computation
    emit suspendProgress();
    break;
  case ctkDicomAppHosting::CANCELED:
    //stop and release everything.
    if (d_ptr->currentState != ctkDicomAppHosting::INPROGRESS
        || d_ptr->currentState != ctkDicomAppHosting::SUSPENDED)
    {
      result = false;
    }
    else
    {
      //releasing resources
      emit cancelProgress();
      //special state, a transitional state, so we notify straight away the new state.
      getHostInterface()->notifyStateChanged(ctkDicomAppHosting::CANCELED);
      d_ptr->currentState = ctkDicomAppHosting::CANCELED;
    }
    break;
  case ctkDicomAppHosting::EXIT:
    //check if current state is IDLE
    if (d_ptr->currentState != ctkDicomAppHosting::IDLE)
    {
      qDebug()<<"illegal transition to EXIT." <<
                 "Current state is:" << d_ptr->currentState;
      result = false;
    }
    //maybe not useful:
    getHostInterface()->notifyStateChanged(ctkDicomAppHosting::EXIT);
    emit exitHostedApp();
    break;
  default:
    //should never happen
    qDebug() << "unexisting state Code, do nothing";
    result = false;
  }
  if (!result)
  {
    qDebug()<<"illegal transition to: "<< newState <<
               "Current state is:" << d_ptr->currentState;
  }
  return result;
}

//----------------------------------------------------------------------------
ctkDicomHostInterface* ctkDicomAbstractApp::getHostInterface() const
{
  ctkDicomHostInterface* host = d_ptr->HostTracker.getService();
  if (!host) throw std::runtime_error("DICOM Host Interface not available");
  return host;
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomAbstractApp::getData(
  const QList<QUuid>& objectUUIDs,
  const QList<QString>& acceptableTransferSyntaxUIDs,
  bool includeBulkData)
{
  return this->objectLocatorCache()->getData(objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
}

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCache* ctkDicomAbstractApp::objectLocatorCache()const
{
  Q_D(const ctkDicomAbstractApp);
  return const_cast<ctkDicomObjectLocatorCache*>(&d->ObjectLocatorCache);
}

//----------------------------------------------------------------------------
bool ctkDicomAbstractApp::publishData(const ctkDicomAppHosting::AvailableData& availableData, bool lastData)
{
  if (!this->objectLocatorCache()->isCached(availableData))
    {
    return false;
    }
  bool success = this->getHostInterface()->notifyDataAvailable(availableData, lastData);
  if(!success)
    {
    return false;
    }
  return true;
}
