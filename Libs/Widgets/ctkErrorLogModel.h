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

#ifndef __ctkErrorLogModel_h
#define __ctkErrorLogModel_h

// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkErrorLogAbstractModel.h"

//------------------------------------------------------------------------------
class ctkErrorLogModelPrivate;

//------------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkErrorLogModel : public ctkErrorLogAbstractModel
{
  Q_OBJECT
public:
  typedef ctkErrorLogAbstractModel Superclass;
  typedef ctkErrorLogModel Self;
  explicit ctkErrorLogModel(QObject* parentObject = 0);
  virtual ~ctkErrorLogModel();

protected:
  QScopedPointer<ctkErrorLogModelPrivate> d_ptr;

  virtual void addModelEntry(const QString& currentDateTime, const QString& threadId,
                             const QString& logLevel, const QString& origin, const QString& text);

private:
  Q_DECLARE_PRIVATE(ctkErrorLogModel)
  Q_DISABLE_COPY(ctkErrorLogModel)
};

#endif
