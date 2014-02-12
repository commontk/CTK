/*=============================================================================

  Library: XNAT/Core

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

#ifndef ctkXnatException_h
#define ctkXnatException_h

#include "ctkXNATCoreExport.h"

#include "ctkException.h"

/// @ingroup XNAT_Core
/// @{
CTK_DECLARE_EXCEPTION(CTK_XNAT_CORE_EXPORT, ctkXnatException, ctkRuntimeException)
CTK_DECLARE_EXCEPTION(CTK_XNAT_CORE_EXPORT, ctkXnatTimeoutException, ctkXnatException)
CTK_DECLARE_EXCEPTION(CTK_XNAT_CORE_EXPORT, ctkXnatInvalidSessionException, ctkXnatException)
CTK_DECLARE_EXCEPTION(CTK_XNAT_CORE_EXPORT, ctkXnatAuthenticationException, ctkXnatException)
CTK_DECLARE_EXCEPTION(CTK_XNAT_CORE_EXPORT, ctkXnatProtocolFailureException, ctkXnatException)
/// @}

#endif
