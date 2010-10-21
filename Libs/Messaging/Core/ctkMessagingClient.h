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

#ifndef __ctkMessagingClient_h
#define __ctkMessagingClient_h

// ZMQ includes 
#include <zmq.hpp>
#include "ctkMessagingCoreExport.h"

//class ctkMessagingClientPrivate;

class CTK_MESSAGING_CORE_EXPORT ctkMessagingClient
{
public:
  //typedef QObject Superclass;
  explicit ctkMessagingClient();
  virtual ~ctkMessagingClient();
  

//protected:
//  QScopedPointer<ctkMessagingClientPrivate> d_ptr;
//private:
//  Q_DECLARE_PRIVATE(ctkMessagingClient);
//  Q_DISABLE_COPY(ctkMessagingClient);

};

#endif
