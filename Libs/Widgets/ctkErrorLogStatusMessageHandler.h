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

#ifndef __ctkErrorLogStatusMessageHandler_h
#define __ctkErrorLogStatusMessageHandler_h

// CTK includes
#include <ctkErrorLogAbstractMessageHandler.h>
#include "ctkWidgetsExport.h"

// Qt includes
#include <QPointer>

class QMainWindow;

//------------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkErrorLogStatusMessageHandler :
    public ctkErrorLogAbstractMessageHandler
{
  Q_OBJECT
public:
  ctkErrorLogStatusMessageHandler(QMainWindow * mainWindow);

  static QString HandlerName;

  virtual QString handlerName()const;
  virtual void setEnabledInternal(bool value);

public Q_SLOTS:
  void statusBarMessageChanged(const QString& text);

private:
  QPointer<QMainWindow> MainWindow;
};

#endif

