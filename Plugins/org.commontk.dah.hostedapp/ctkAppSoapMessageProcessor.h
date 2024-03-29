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

#ifndef CTKAPPSOAPMESSAGEPROCESSOR_H
#define CTKAPPSOAPMESSAGEPROCESSOR_H

// CTK includes
#include "ctkSoapMessageProcessor.h"
#include "ctkDicomAppInterface.h"

class ctkAppSoapMessageProcessor : public ctkSoapMessageProcessor
{

public:

  ctkAppSoapMessageProcessor( ctkDicomAppInterface* inter );

  virtual bool process(
    const QtSoapMessage& message,
    QtSoapMessage* reply) const;

private:

  void processGetState(const QtSoapMessage& message,
                       QtSoapMessage* reply) const;
  void processSetState(const QtSoapMessage& message,
                       QtSoapMessage* reply) const;
  void processBringToFront(const QtSoapMessage& message,
                           QtSoapMessage* reply) const;

  ctkDicomAppInterface* AppInterface;

};

#endif // CTKSOAPMESSAGEPROCESSORLIST_H
