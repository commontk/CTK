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

#ifndef __ctkErrorLogStreamMessageHandler_h
#define __ctkErrorLogStreamMessageHandler_h

// CTK includes
#include <ctkErrorLogAbstractMessageHandler.h>
#include "ctkCoreExport.h"

class ctkErrorLogStreamMessageHandlerPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogStreamMessageHandler
  : public ctkErrorLogAbstractMessageHandler
{
public:
  typedef ctkErrorLogAbstractMessageHandler Superclass;

  ctkErrorLogStreamMessageHandler();
  virtual ~ctkErrorLogStreamMessageHandler();

  static QString HandlerName;

  virtual QString handlerName()const;
  virtual void setEnabledInternal(bool value);

protected:
  QScopedPointer<ctkErrorLogStreamMessageHandlerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogStreamMessageHandler);
  Q_DISABLE_COPY(ctkErrorLogStreamMessageHandler);
};

#endif

