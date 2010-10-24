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

#include <ctkDicomHostInterface.h>
#include <ctkDicomAppInterface.h>
#include <QScopedPointer>

#include <org_commontk_dah_host_Export.h>

class ctkDicomAbstractHostPrivate;

/**
  * Provide a basic implementation for an application host.
  *
  * It starts a http server and serves one hosted application. Multiple instances
  * can be used for hosting multiple applications.
  *
  * The methods of the ctkDicomHostInterface have to be implemented for the business logic,
  *
  */
class org_commontk_dah_host_EXPORT ctkDicomAbstractHost : public ctkDicomHostInterface
{

public:

  /**
    * Start the soap sever on the specified port or choose port automatically.
    */
  ctkDicomAbstractHost(int hostPort = 0, int appPort = 0);
  int getHostPort() const;
  int getAppPort() const;
  ~ctkDicomAbstractHost();

  ctkDicomAppInterface* getDicomAppService() const;

private:

  Q_DECLARE_PRIVATE(ctkDicomAbstractHost)
  const QScopedPointer<ctkDicomAbstractHostPrivate> d_ptr;

};

#endif // CTKDICOMABSTRACTHOST_H
