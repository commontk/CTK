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


#ifndef CTKCMDLINEMODULEEXPLORERSHOWXMLACTION_H
#define CTKCMDLINEMODULEEXPLORERSHOWXMLACTION_H

#include "ctkCmdLineModuleReference.h"

#include <QAction>

class ctkCmdLineModuleExplorerShowXmlAction : public QAction
{
  Q_OBJECT

public:

  ctkCmdLineModuleExplorerShowXmlAction(QObject* parent);

  void setModuleReference(const ctkCmdLineModuleReference& ref);

protected:

  Q_SLOT virtual void run();

private:

  ctkCmdLineModuleReference ModuleRef;
};

#endif // CTKCMDLINEMODULEEXPLORERSHOWXMLACTION_H
