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

=========================================================================*/

#ifndef __ctkVTKObjectTestHelper_h
#define __ctkVTKObjectTestHelper_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

// CTKVTK includes
#include "ctkVTKObject.h"

class ctkVTKObjectTestPrivate;

class ctkVTKObjectTest: public QObject
{
  Q_OBJECT
  QVTK_OBJECT
public:
  ctkVTKObjectTest(QObject* parent = 0);
  virtual ~ctkVTKObjectTest();

  bool test();

  bool isPublicSlotCalled()const;
  bool isProtectedSlotCalled()const;
  bool isPrivateSlotCalled()const;

  void resetSlotCalls();

  void emitSignalEmitted();
public Q_SLOTS:
  void onVTKObjectModifiedPublic();
  void deleteConnection();

protected Q_SLOTS:
  void onVTKObjectModifiedProtected();

private Q_SLOTS:
  void onVTKObjectModifiedPrivate();

Q_SIGNALS:
  void signalEmitted();

protected:
  QScopedPointer<ctkVTKObjectTestPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKObjectTest);
  Q_DISABLE_COPY(ctkVTKObjectTest);
};

#endif
