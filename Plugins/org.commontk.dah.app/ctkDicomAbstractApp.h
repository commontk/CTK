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

#ifndef CTKDICOMABSTRACTAPP_H
#define CTKDICOMABSTRACTAPP_H

#include <ctkDicomAppInterface.h>
#include <QScopedPointer>

class ctkDicomAbstractAppPrivate;

/**
  * Provide a basic implementation for an application app.
  *
  * It starts a http server and serves one hosted application. Multiple instances
  * can be used for hosting multiple applications.
  *
  * The methods of the ctkDicomAppInterface have to be implemented for the business logic,
  *
  */
class ctkDicomAbstractApp : public ctkDicomAppInterface
{

public:

  /**
   * Start the soap sever on the specified port or choose port automatically.
   */
  ctkDicomAbstractApp(int port = 0);
  int getPort() const;
  ~ctkDicomAbstractApp();

private:
  Q_DECLARE_PRIVATE(ctkDicomAbstractApp)
  const QScopedPointer<ctkDicomAbstractAppPrivate> d_ptr;

};

#endif // CTKDICOMABSTRACTAPP_H
