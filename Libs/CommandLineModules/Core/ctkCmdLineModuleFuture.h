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

#include "ctkCommandLineModulesCoreExport.h"

#include "ctkCmdLineModuleFutureInterface.h"

#include <QFuture>

/**
 * \class ctkCmdLineModuleFuture
 * \brief QFuture sub-class for enhanced communication with running modules.
 * \ingroup CommandLineModulesCore_API
 *
 * Please see the QFuture documentation of Qt for details. This sub-class provides
 * additional query methods to check if a module can be paused and/or canceled and
 * also provides the ability to retrieve the arbitrary output and error data
 * from the module.
 *
 * \see ctkCmdLineModuleFutureWatcher
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFuture : public QFuture<ctkCmdLineModuleResult>
{
public:

  ctkCmdLineModuleFuture();

  /** \cond */
  explicit ctkCmdLineModuleFuture(ctkCmdLineModuleFutureInterface* p); // internal
  /** \endcond */

  /**
   * @brief Read all output data reported by the running module so far.
   * @return Returns the reported output.
   */
  QByteArray readAllOutputData() const;

  /**
   * @brief Read all error data reported by the running module so far.
   * @return Returns the reported error.
   */
  QByteArray readAllErrorData() const;

  /**
   * @brief Check if this module can be canceled via cancel().
   * @return \c true if this module can be canceled, \c false otherwise.
   */
  bool canCancel() const;

  /**
   * @brief Check if this module can be paused via pause() and similar QFuture methods.
   * @return \c true if this module can be paused, \c false otherwise.
   */
  bool canPause() const;

};

inline ctkCmdLineModuleFuture ctkCmdLineModuleFutureInterface::future()
{
  return ctkCmdLineModuleFuture(this);
}

#endif // CTKCMDLINEMODULEFUTURE_H
