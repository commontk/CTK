/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

#ifndef __ctkDICOMStorageListenerJobPrivate_h
#define __ctkDICOMStorageListenerJobPrivate_h

// ctkDICOMCore includes
#include "ctkDICOMStorageListenerJob.h"

//------------------------------------------------------------------------------
class ctkDICOMStorageListenerJobPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkDICOMStorageListenerJob)

protected:
  ctkDICOMStorageListenerJob* const q_ptr;

public:
  ctkDICOMStorageListenerJobPrivate(ctkDICOMStorageListenerJob* object);
  virtual ~ctkDICOMStorageListenerJobPrivate();

  QString AETitle;
  int Port;
  int ConnectionTimeout;
};

#endif
