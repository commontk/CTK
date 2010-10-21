/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// ZMQ includes

// ctkMessagingCore includes
#include "ctkMessagingServer.h"

// STD includes
#include <iostream>


//------------------------------------------------------------------------------
ctkMessagingServer::ctkMessagingServer()/*:d_ptr(new ctkMessagingServerPrivate)*/
{
  zmq::context_t ctx (1, 1);

  // Create a PUB socket for port 5555 on the lo interface
  zmq::socket_t s(ctx, ZMQ_PUB);
  //s.bind ("tcp://lo0:5555");
}

//----------------------------------------------------------------------------
ctkMessagingServer::~ctkMessagingServer()
{
}

