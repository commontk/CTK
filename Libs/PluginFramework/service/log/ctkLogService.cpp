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


#include "ctkLogService.h"

const int ctkLogService::LOG_ERROR = 1;
const int ctkLogService::LOG_WARNING = 2;
const int ctkLogService::LOG_INFO = 3;
const int ctkLogService::LOG_DEBUG = 4;

//----------------------------------------------------------------------------
ctkLogStreamWithServiceRef::ctkLogStreamWithServiceRef(ctkLogService* logService, const ctkServiceReference& sr,
                                                       int level, const std::exception* exc, const char* file,
                                                       const char* function, int line)
  : ctkLogStream(logService, level, exc, file, function, line), sr(sr)
{

}

//----------------------------------------------------------------------------
ctkLogStreamWithServiceRef::ctkLogStreamWithServiceRef(const ctkLogStreamWithServiceRef &logStreamWithRef)
 : ctkLogStream(logStreamWithRef), sr(logStreamWithRef.sr)
{

}

//----------------------------------------------------------------------------
ctkLogStreamWithServiceRef::~ctkLogStreamWithServiceRef()
{
  if (!logged)
  {
    logService->log(sr, level, msg, exc, file, function, line);
    logged = true;
  }
}

//----------------------------------------------------------------------------
ctkNullLogStream::ctkNullLogStream() : ctkLogStream(0, 0)
{}

//----------------------------------------------------------------------------
ctkNullLogStream::~ctkNullLogStream()
{
  logged = true;
}
