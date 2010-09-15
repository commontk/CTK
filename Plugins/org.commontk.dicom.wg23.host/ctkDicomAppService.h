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


#ifndef CTKDICOMHOSTSERVICE_H
#define CTKDICOMHOSTSERVICE_H

#include <ctkDicomAppInterface.h>
#include <QScopedPointer>
#include <org_commontk_dicom_wg23_host_Export.h>

class ctkDicomServicePrivate;

class org_commontk_dicom_wg23_host_EXPORT ctkDicomAppService : public ctkDicomAppInterface
{

public:
  ctkDicomAppService(int port);
  ~ctkDicomAppService();

  ctkDicomWG23::State getState();
  bool setState(ctkDicomWG23::State newState);
  bool bringToFront(const QRect& requestedScreenArea);

private:
  Q_DECLARE_PRIVATE(ctkDicomService)

  const QScopedPointer<ctkDicomServicePrivate> d_ptr;
};

#endif // CTKDICOMHOSTSERVICE_H
