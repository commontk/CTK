/*=============================================================================

  Plugin: org.commontk.xnat

  Copyright (c) University College London,
    Centre for Medical Image Computing

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

#include "ctkXnatException.h"

CTK_IMPLEMENT_EXCEPTION(ctkXnatTimeoutException, ctkRuntimeException, "ctkXnatTimeoutException")
CTK_IMPLEMENT_EXCEPTION(ctkXnatSessionException, ctkRuntimeException, "ctkXnatSessionException")
CTK_IMPLEMENT_EXCEPTION(ctkXnatAuthenticationException, ctkXnatSessionException, "ctkXnatAuthenticationException")
CTK_IMPLEMENT_EXCEPTION(ctkXnatProtocolFailureException, ctkRuntimeException, "ctkXnatProtocolFailureException")
