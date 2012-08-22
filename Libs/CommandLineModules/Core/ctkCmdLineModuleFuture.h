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

#ifndef CTKCMDLINEMODULEFUTURE_H
#define CTKCMDLINEMODULEFUTURE_H

#include "ctkCmdLineModuleFutureInterface.h"

#include <QFuture>

/**
 * \class ctkCmdLineModuleFuture
 * \brief QFuture sub-class with in addition canCancel() and canPause().
 * \ingroup CommandLineModulesCore
 *
 *
 * QFuture sub-class with two additional methods:
 *
 *   - bool canCancel()
 *   - bool canPause()
 */
class ctkCmdLineModuleFuture : public QFuture<ctkCmdLineModuleResult>
{
public:

  ctkCmdLineModuleFuture()
  {
  }

  explicit ctkCmdLineModuleFuture(ctkCmdLineModuleFutureInterface* p) // internal
    : QFuture<ctkCmdLineModuleResult>(p)
  {
  }

  bool canCancel() const { return  d.canCancel(); }
  bool canPause() const { return d.canPause(); }

};

inline ctkCmdLineModuleFuture ctkCmdLineModuleFutureInterface::future()
{
  return ctkCmdLineModuleFuture(this);
}

#endif // CTKCMDLINEMODULEFUTURE_H
