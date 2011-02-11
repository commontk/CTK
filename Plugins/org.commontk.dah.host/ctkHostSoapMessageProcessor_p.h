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

#ifndef ctkHostSoapMessageProcessor_H
#define ctkHostSoapMessageProcessor_H

#include "ctkSoapMessageProcessor.h"
#include "ctkDicomHostInterface.h"

class ctkHostSoapMessageProcessor : public ctkSoapMessageProcessor
{

public:

  ctkHostSoapMessageProcessor( ctkDicomHostInterface* inter );

  virtual bool process(
    const QtSoapMessage& message,
    QtSoapMessage* reply) const;
    
private:

  void processGetAvailableScreen(const QtSoapMessage& message,
                                 QtSoapMessage* reply) const;
  void processNotifyStateChanged(const QtSoapMessage& message,
                                 QtSoapMessage* reply) const;
  void processNotifyStatus(const QtSoapMessage& message,
                                 QtSoapMessage* reply) const;
  void processGenerateUID(const QtSoapMessage& message,
                                 QtSoapMessage* reply) const;
  void processGetOutputLocation(const QtSoapMessage& message,
                                 QtSoapMessage* reply) const;
               
  ctkDicomHostInterface* HostInterface;

};

#endif // CTKSOAPMESSAGEPROCESSORLIST_H
