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

#ifndef CTKCMDLINEMODULEBACKENDFPTYPETRAITS_H
#define CTKCMDLINEMODULEBACKENDFPTYPETRAITS_H

namespace ctk {
namespace CmdLineModuleBackendFunctionPointer {

struct NullType {};

template<bool flag, typename T, typename U>
struct Select
{
  typedef T Result;
};

template<typename T, typename U>
struct Select<false, T, U>
{
  typedef U Result;
};

template<typename T>
class TypeTraits
{
private:

  template<class U> struct PointerTraits
  {
    enum { result = false };
    typedef NullType PointeeType;
  };
  template<class U> struct PointerTraits<U*>
  {
    enum { result = true };
    typedef U PointeeType;
  };
  template<class U> struct ReferenceTraits
  {
    enum { result = false };
    typedef NullType ReferenceType;
  };
  template<class U> struct ReferenceTraits<U&>
  {
    enum { result = true };
    typedef U ReferenceType;
  };

  template<class U> struct UnConst
  {
    typedef U Result;
  };
  template<class U> struct UnConst<const U>
  {
    typedef U Result;
  };

public:

  typedef typename PointerTraits<T>::PointeeType PointeeType;
  typedef typename ReferenceTraits<T>::ReferenceType ReferenceType;

  enum { isPointer = PointerTraits<T>::result };
  enum { isReference = ReferenceTraits<T>::result };

  typedef typename Select<isPointer, typename UnConst<PointeeType>::Result,
                                     typename Select<isReference, typename UnConst<ReferenceType>::Result, typename UnConst<T>::Result>::Result >::Result RawType;
};

template<bool C, typename T = void>
struct EnableIf
{
  typedef T Type;
};

template<typename T>
struct EnableIf<false, T> {};

template<typename, typename>
struct IsSame
{
  static bool const value = false;
};

template<typename A>
struct IsSame<A, A>
{
  static bool const value = true;
};

template<typename B, typename D>
struct IsBaseOf
{
  static D* MakeD();
  static char (& Test(B*))[1];
  static char (& Test(...))[2];
  static bool const value = sizeof Test(MakeD()) == 1 &&
      !IsSame<B volatile const, void volatile const>::value;
};


}
}


#endif // CTKCMDLINEMODULEBACKENDFPTYPETRAITS_H
