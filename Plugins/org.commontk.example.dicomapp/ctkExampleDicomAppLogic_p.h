/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#ifndef CTKEXAMPLEDICOMAPPLOGIC_P_H
#define CTKEXAMPLEDICOMAPPLOGIC_P_H

#include <ctkPluginActivator.h>
#include <ctkDicomAppInterface.h>
#include <ctkDicomHostInterface.h>
#include <stdexcept>

template <class TServiceType>
class ServiceAccessor {
public:
  ServiceAccessor(ctkPluginContext* context, const QString& clazz) : context(context), clazz(clazz)
  {
  }
  TServiceType* operator->()
  {
    ctkServiceReference* serviceRef = context->getServiceReference(clazz);
    if (!serviceRef)
    {
      // this will change after merging changes from branch plugin_framework
      throw std::runtime_error("No Dicom Host Service found");
    }
    return qobject_cast<TServiceType*>(context->getService(serviceRef));
  }
  TServiceType* operator*()
  {
    return operator->();
  }
private:
  ctkPluginContext* context;
  const QString clazz;
};


class ctkExampleDicomAppLogic :
  public ctkDicomAppInterface
{
  Q_OBJECT

public:

  ctkExampleDicomAppLogic(ServiceAccessor<ctkDicomHostInterface> host);
  ~ctkExampleDicomAppLogic();

  // ctkDicomAppInterface
  ctkDicomWG23::State getState();
  bool setState(ctkDicomWG23::State newState);
  bool bringToFront(const QRect& requestedScreenArea);

  // ctkDicomExchangeInterface

  // some logic
  void do_something();
private:
  ServiceAccessor<ctkDicomHostInterface> host;

}; // ctkExampleDicomAppLogic

#endif // ctkExampleDicomAppLogic_P_H
