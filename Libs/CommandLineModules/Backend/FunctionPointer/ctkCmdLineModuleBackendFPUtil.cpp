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

#include "ctkCmdLineModuleBackendFPUtil_p.h"


namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

//----------------------------------------------------------------------------
FunctionPointerHolderBase::~FunctionPointerHolderBase()
{
}

//----------------------------------------------------------------------------
FunctionPointerProxy::FunctionPointerProxy()
  : FpHolder(NULL)
{}

//----------------------------------------------------------------------------
FunctionPointerProxy::~FunctionPointerProxy()
{
  delete FpHolder;
}

//----------------------------------------------------------------------------
FunctionPointerProxy::FunctionPointerProxy(const FunctionPointerProxy& other)
  : FpHolder(other.FpHolder->clone())
{
}

//----------------------------------------------------------------------------
FunctionPointerProxy& FunctionPointerProxy::operator=(const FunctionPointerProxy& other)
{
  delete this->FpHolder;
  this->FpHolder = other.FpHolder->clone();
  return *this;
}

//----------------------------------------------------------------------------
void FunctionPointerProxy::call(const QList<QVariant> &args)
{
  FpHolder->call(args);
}

}
}
