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
  * TODO: provide helper/convenience methods to ease application development
  *
  * The methods of the ctkDicomAppInterface have to be implemented for the business logic,
  *
  */
class ctkDicomAbstractApp : public ctkDicomAppInterface
{

public:

  ctkDicomAbstractApp();
  ~ctkDicomAbstractApp();

private:
  Q_DECLARE_PRIVATE(ctkDicomAbstractApp)
  const QScopedPointer<ctkDicomAbstractAppPrivate> d_ptr;

};

#endif // CTKDICOMABSTRACTAPP_H
