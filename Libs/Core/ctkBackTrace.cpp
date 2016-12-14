/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

//
// Code taken from http://thread.gmane.org/gmane.comp.lib.boost.devel/209982
// and modified for CTK.
//
// Original Copyright (c) 2010 Artyom Beilis (Tonkikh)
//
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//

#include "ctkBackTrace.h"

#include <QList>

#include <vector>

#if defined(__linux) || defined(__APPLE__) || defined(__sun)
#define CTK_HAVE_EXECINFO
#define CTK_HAVE_DLADDR
#endif

#if defined(__GNUC__)
#define CTK_HAVE_ABI_CXA_DEMANGLE
#endif

#ifdef CTK_HAVE_EXECINFO
#include <execinfo.h>
#endif

#ifdef CTK_HAVE_ABI_CXA_DEMANGLE
#include <cxxabi.h>
#endif

#ifdef CTK_HAVE_DLADDR
#include <dlfcn.h>
#endif

#include <stdlib.h>
#include <sstream>

#if defined(Q_CC_MSVC)
#include <windows.h>
#include <stdlib.h>
#include <dbghelp.h>
#endif

// --------------------------------------------------------------------------
size_t const ctkBackTrace::DefaultStackSize = 32;

// --------------------------------------------------------------------------
struct ctkBackTracePrivate
{
  std::vector<void *> Frames;

  int trace(void** addresses, size_t size) const;
  std::string getSymbol(void* address) const;
};

// --------------------------------------------------------------------------
ctkBackTrace::ctkBackTrace(const ctkBackTrace& other)
  : d(new ctkBackTracePrivate(*other.d.data()))
{
}

ctkBackTrace::ctkBackTrace(size_t framesNumber)
  : d(new ctkBackTracePrivate)
{
  if(framesNumber == 0)
    return;
  d->Frames.resize(framesNumber, 0);
  size_t size = d->trace(&d->Frames.front(), framesNumber);
  d->Frames.resize(size);
}

// --------------------------------------------------------------------------
ctkBackTrace::~ctkBackTrace() throw()
{
}

// --------------------------------------------------------------------------
size_t ctkBackTrace::stackSize() const
{
  return d->Frames.size();
}

// --------------------------------------------------------------------------
void* ctkBackTrace::returnAddress(unsigned frameNumber) const
{
  if(frameNumber < stackSize())
    return d->Frames[frameNumber];
  return 0;
}

// --------------------------------------------------------------------------
QString ctkBackTrace::stackFrame(unsigned frameNumber) const
{
  if(frameNumber < d->Frames.size())
    return QString::fromStdString(d->getSymbol(d->Frames[frameNumber]));
  return QString();
}

// --------------------------------------------------------------------------
QList<QString> ctkBackTrace::stackTrace() const
{
  QList<QString> trace;

  if(d->Frames.empty())
    return trace;

  for (std::size_t i = 0; i < d->Frames.size(); ++i)
  {
    std::string s = d->getSymbol(d->Frames[i]);
    if (!s.empty())
    {
      trace.push_back(QString::fromStdString(s));
    }
  }

  return trace;

  //std::ostringstream res;
  //d->writeSymbols(&d->Frames.front(), d->Frames.size(), res, framePrefix.toStdString());
  //return QString::fromStdString(res.str());
}

#if defined(CTK_HAVE_EXECINFO)

// --------------------------------------------------------------------------
int ctkBackTracePrivate::trace(void** array, size_t n) const
{
  return :: backtrace(array,n);
}

#elif defined(Q_CC_MSVC)

USHORT (WINAPI *s_pfnCaptureStackBackTrace)(ULONG, ULONG, PVOID*, PULONG) = 0;

// --------------------------------------------------------------------------
int ctkBackTracePrivate::trace(void** array, size_t n) const
{
  if(n>=63)
    n=62;

  if (s_pfnCaptureStackBackTrace == 0)
  {
    const HMODULE hNtDll = ::GetModuleHandleW(L"ntdll.dll");
    reinterpret_cast<void*&>(s_pfnCaptureStackBackTrace) =
        ::GetProcAddress(hNtDll, "RtlCaptureStackBackTrace");
  }

  if (s_pfnCaptureStackBackTrace != 0) {
    return s_pfnCaptureStackBackTrace(0, static_cast<ULONG>(n), array, 0);
  }
  return 0;
}

#else

// --------------------------------------------------------------------------
int ctkBackTracePrivate::trace(void** /*array*/, size_t /*n*/) const
{
  return 0;
}

#endif

#if defined(CTK_HAVE_DLADDR) && defined(CTK_HAVE_ABI_CXA_DEMANGLE)

// --------------------------------------------------------------------------
std::string ctkBackTracePrivate::getSymbol(void* ptr) const
{
  if(!ptr)
    return std::string();

  std::ostringstream res;
  res.imbue(std::locale::classic());
  res << ptr << ": ";
  Dl_info info = {0,0,0,0};
  if(dladdr(ptr,&info) == 0)
  {
    res << "???";
  }
  else
  {
    if(info.dli_sname)
    {
      int status = 0;
      char *demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
      if(demangled)
      {
        res << demangled;
        free(demangled);
      }
      else
      {
        res << info.dli_sname;
      }
    }
    else
    {
      res << "???";
    }

    unsigned offset = reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(info.dli_saddr);
    res << std::hex <<" + 0x" << offset ;

    if(info.dli_fname)
      res << " in " << info.dli_fname;
  }
  return res.str();
}

#elif defined(CTK_HAVE_EXECINFO)
// --------------------------------------------------------------------------
std::string ctkBackTracePrivate::getSymbol(void *address) const
{
  char ** ptr = backtrace_symbols(&address, 1);
  try
  {
    if(ptr == 0)
      return std::string();
    std::string res = ptr[0];
    free(ptr);
    ptr = 0;
    return res;
  }
  catch(...)
  {
    free(ptr);
    throw;
  }
}

#elif defined(Q_CC_MSVC)

// --------------------------------------------------------------------------
namespace {
HANDLE hProcess = 0;
bool syms_ready = false;
}

// --------------------------------------------------------------------------
namespace ctk {
bool DebugSymInitialize()
{
  if(hProcess == 0)
  {
    hProcess = GetCurrentProcess();
    SymSetOptions(SYMOPT_DEFERRED_LOADS);

    if (SymInitialize(hProcess, NULL, TRUE))
    {
      syms_ready = true;
    }
  }
  return syms_ready;
}
}

// --------------------------------------------------------------------------
std::string ctkBackTracePrivate::getSymbol(void* ptr) const
{
  if(ptr==0)
    return std::string();

  ctk::DebugSymInitialize();
  std::ostringstream ss;
  ss.imbue(std::locale::classic());
  ss << ptr;
  if(syms_ready)
  {
    DWORD64  dwDisplacement = 0;
    DWORD64  dwAddress = (DWORD64)ptr;

    std::vector<char> buffer(sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)&buffer.front();

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    if (SymFromAddr(hProcess, dwAddress, &dwDisplacement, pSymbol))
    {
      ss <<": " << pSymbol->Name << std::hex << " + 0x" << dwDisplacement;
    }
    else
    {
      ss << ": ???";
    }

    std::vector<char> moduleBuffer(sizeof(IMAGEHLP_MODULE64));
    PIMAGEHLP_MODULE64 pModuleInfo = (PIMAGEHLP_MODULE64)&moduleBuffer.front();
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    if (SymGetModuleInfo64(hProcess, pSymbol->ModBase, pModuleInfo))
    {
      ss << " in " << pModuleInfo->LoadedImageName;
    }
  }
  return ss.str();
}

#else

// --------------------------------------------------------------------------
std::string ctkBackTracePrivate::getSymbol(void* ptr) const
{
  if(!ptr)
    return std::string();

  std::ostringstream res;
  res.imbue(std::locale::classic());
  res << ptr;
  return res.str();
}

#endif
