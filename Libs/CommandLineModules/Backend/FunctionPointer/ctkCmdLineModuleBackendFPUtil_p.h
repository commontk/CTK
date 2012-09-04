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

#ifndef CTKCMDLINEMODULEBACKENDFPUTIL_P_H
#define CTKCMDLINEMODULEBACKENDFPUTIL_P_H

#include "ctkCommandLineModulesBackendFunctionPointerExport.h"

#include <QVariant>

class ctkCmdLineModuleBackendFunctionPointer;

namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

struct CTK_CMDLINEMODULEBACKENDFP_EXPORT FunctionPointerHolderBase
{
  virtual ~FunctionPointerHolderBase();

  virtual FunctionPointerHolderBase* clone() const = 0;

  virtual void call(const QList<QVariant>& args) = 0;
};


template<typename A>
struct FunctionPointerHolder : public FunctionPointerHolderBase
{
  typedef void (*FunctionPointerType)(A);

  FunctionPointerHolder(FunctionPointerType fp) : Fp(fp) {}

  FunctionPointerHolderBase* clone() const
  {
    return new FunctionPointerHolder(*this);
  }

  void call(const QList<QVariant>& args)
  {
    Q_ASSERT(args.size() > 0);
    Q_ASSERT(args.at(0).canConvert<A>());
    Fp(args.at(0).value<A>());
  }

  FunctionPointerType Fp;
};

template<typename A, typename B>
struct FunctionPointerHolder2 : public FunctionPointerHolderBase
{
  typedef void (*FunctionPointerType)(A,B);

  FunctionPointerHolder2(FunctionPointerType fp) : Fp(fp) {}

  FunctionPointerHolderBase* clone() const
  {
    return new FunctionPointerHolder2(*this);
  }

  void call(const QList<QVariant>& args)
  {
    Q_ASSERT(args.size() > 1);
    Q_ASSERT(args.at(0).canConvert<A>());
    Q_ASSERT(args.at(1).canConvert<B>());
    Fp(args.at(0).value<A>(), args.at(1).value<B>());
  }

  FunctionPointerType Fp;
};

struct CTK_CMDLINEMODULEBACKENDFP_EXPORT FunctionPointerProxy
{
  FunctionPointerProxy();
  ~FunctionPointerProxy();

  FunctionPointerProxy(const FunctionPointerProxy& other);
  FunctionPointerProxy& operator=(const FunctionPointerProxy& other);

  template<typename A>
  FunctionPointerProxy(void (*fp)(A))
    : FpHolder(new FunctionPointerHolder<A>(fp)) {}

  template<typename A, typename B>
  FunctionPointerProxy(void (*fp)(A,B))
    : FpHolder(new FunctionPointerHolder2<A,B>(fp)) {}

  void call(const QList<QVariant>& args);

private:

  friend class ::ctkCmdLineModuleBackendFunctionPointer;

  FunctionPointerHolderBase* FpHolder;
};

}
}

#endif // CTKCMDLINEMODULEBACKENDFPUTIL_P_H
