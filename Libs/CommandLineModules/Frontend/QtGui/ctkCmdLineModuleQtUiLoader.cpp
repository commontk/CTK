/*=============================================================================

  Library: CTK

  Copyright (c) University College London

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

#include "ctkCmdLineModuleQtUiLoader.h"

#include <ctkCmdLineModuleQtComboBox_p.h>

//-----------------------------------------------------------------------------
ctkCmdLineModuleQtUiLoader::ctkCmdLineModuleQtUiLoader(QObject *parent)
  : QUiLoader(parent)
{

}

//-----------------------------------------------------------------------------
ctkCmdLineModuleQtUiLoader::~ctkCmdLineModuleQtUiLoader()
{

}


//-----------------------------------------------------------------------------
QWidget* ctkCmdLineModuleQtUiLoader::createWidget(const QString& className, QWidget* parent, const QString& name)
{
  QWidget* widget = NULL;

  if (className == "QComboBox")
  {
    widget = new ctkCmdLineModuleQtComboBox(parent);
    widget->setObjectName(name);
  }
  else
  {
    widget = QUiLoader::createWidget(className, parent, name);
  }

  return widget;
}
