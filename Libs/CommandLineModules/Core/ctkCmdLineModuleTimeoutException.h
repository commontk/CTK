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

#ifndef CTKCMDLINEMODULETIMEOUTEXCEPTION_H
#define CTKCMDLINEMODULETIMEOUTEXCEPTION_H

#include <ctkException.h>

#include "ctkCommandLineModulesCoreExport.h"

/**
 * \class ctkCmdLineModuleTimeoutException
 * \brief Exception class to describe problems with time-outs.
 * \ingroup CommandLineModulesCore_API
 */
CTK_DECLARE_EXCEPTION(CTK_CMDLINEMODULECORE_EXPORT, ctkCmdLineModuleTimeoutException, ctkException)

#endif // CTKCMDLINEMODULETIMEOUTEXCEPTION_H
