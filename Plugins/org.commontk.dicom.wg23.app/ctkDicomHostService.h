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

#include <ctkDicomHostInterface.h>
#include <QScopedPointer>
#include <org_commontk_dicom_wg23_app_Export.h>

class ctkDicomServicePrivate;

class org_commontk_dicom_wg23_app_EXPORT ctkDicomHostService : public ctkDicomHostInterface
{

public:
  ctkDicomHostService(int port, QString path);
  ~ctkDicomHostService();

  QString generateUID();
  QRect getAvailableScreen(const QRect& preferredScreen);
  QString getOutputLocation(const QStringList& preferredProtocols);
  void notifyStateChanged(ctkDicomWG23::State state);
  void notifyStatus(const ctkDicomWG23::Status& status);

private:
  Q_DECLARE_PRIVATE(ctkDicomService)

  const QScopedPointer<ctkDicomServicePrivate> d_ptr;
};

#endif // CTKDICOMHOSTSERVICE_H
