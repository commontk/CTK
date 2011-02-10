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
#include <ctkPluginContext.h>
#include <ctkServiceTracker.h>

class ctkDicomAbstractAppPrivate
{
public:
  ctkDicomAbstractAppPrivate(ctkPluginContext* context);
  ~ctkDicomAbstractAppPrivate();

  ctkServiceTracker<ctkDicomHostInterface*> HostTracker;
  ctkDicomAppHosting::State currentState;
};

//----------------------------------------------------------------------------
// ctkDicomAbstractAppPrivate methods

//----------------------------------------------------------------------------
ctkDicomAbstractAppPrivate::ctkDicomAbstractAppPrivate(ctkPluginContext * context):HostTracker(context),currentState(ctkDicomAppHosting::IDLE)
{
  //perhaps notStarted or some dummy state instead of IDLE?
}

//----------------------------------------------------------------------------
ctkDicomAbstractAppPrivate::~ctkDicomAbstractAppPrivate()
{
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
  bool result = false;
  //received a new state,
  switch (newState){
  case ctkDicomAppHosting::IDLE:
    if (d_ptr->currentState == ctkDicomAppHosting::COMPLETED)
    {
      emit releaseResources();
      result = true;
    }
    break;
  case ctkDicomAppHosting::INPROGRESS:
    if (d_ptr->currentState == ctkDicomAppHosting::IDLE)
    {
      emit startProgress();
      result = true;
    }
    else if(d_ptr->currentState == ctkDicomAppHosting::SUSPENDED)
    {
      emit resumeProgress();
      result = true;
    }
    break;
  case ctkDicomAppHosting::COMPLETED:
    qDebug() << "Hosting system shouldn't send completed";
    break;
  case ctkDicomAppHosting::SUSPENDED:
    //suspend computation, release as much resource as possible with possible resuming of computation
    emit suspendProgress();
    result = true;
    break;
  case ctkDicomAppHosting::CANCELED:
    //stop and release everything.
    if (d_ptr->currentState == ctkDicomAppHosting::INPROGRESS
        || d_ptr->currentState == ctkDicomAppHosting::SUSPENDED)
    {
      //releasing resources
      emit cancelProgress();
      //special state, a transitional state, so we notify straight away the new state.
      getHostInterface()->notifyStateChanged(ctkDicomAppHosting::CANCELED);
      d_ptr->currentState = ctkDicomAppHosting::CANCELED;
      result = true;
    }
    break;
  case ctkDicomAppHosting::EXIT:
    //check if current state is IDLE
    if (d_ptr->currentState == ctkDicomAppHosting::IDLE)
    {
      //maybe not useful:
      getHostInterface()->notifyStateChanged(ctkDicomAppHosting::EXIT);
      emit exitHostedApp();
      result = true;
    }
    break;
  default:
    //should never happen
    qDebug() << "unexisting state Code, do nothing";
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
ctkDicomAppHosting::State ctkDicomAbstractApp::getState()
{
  return d_ptr->currentState;
}


