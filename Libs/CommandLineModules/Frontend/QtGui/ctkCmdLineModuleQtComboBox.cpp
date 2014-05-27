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

#include "ctkCmdLineModuleQtComboBox_p.h"

//-----------------------------------------------------------------------------
ctkCmdLineModuleQtComboBox::ctkCmdLineModuleQtComboBox(QWidget* parent)
  : QComboBox(parent)
{}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleQtComboBox::setCurrentEnumeration(const QString& text)
{
  int i = findText(text);
  if (i == -1)
  {
    return;
  }
  this->setCurrentIndex(i);
}

//-----------------------------------------------------------------------------
QString ctkCmdLineModuleQtComboBox::currentEnumeration() const
{
  return this->currentText();
}
