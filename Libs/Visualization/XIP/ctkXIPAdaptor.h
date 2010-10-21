/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkXIPAdaptor_h
#define __ctkXIPAdaptor_h

// Qt includes 
#include <QObject>

#include "ctkVisualizationXIPExport.h"

class ctkXIPAdaptorPrivate;
class CTK_VISUALIZATION_XIP_EXPORT ctkXIPAdaptor : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkXIPAdaptor(QObject* parent = 0);
  virtual ~ctkXIPAdaptor();
  
protected:
  QScopedPointer<ctkXIPAdaptorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkXIPAdaptor);
  Q_DISABLE_COPY(ctkXIPAdaptor);
};

#endif
