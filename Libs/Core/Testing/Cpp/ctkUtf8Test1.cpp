/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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


#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#include <iostream>
#include <fstream>

// Helper function to get Windows version
#ifdef _WIN32
typedef LONG NTSTATUS, * PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
RTL_OSVERSIONINFOW GetRealOSVersion()
{
  HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
  if (hMod)
  {
    RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
    if (fxPtr != nullptr)
    {
      RTL_OSVERSIONINFOW rovi = { 0 };
      rovi.dwOSVersionInfoSize = sizeof(rovi);
      if (STATUS_SUCCESS == fxPtr(&rovi))
      {
        return rovi;
      }
    }
  }
  RTL_OSVERSIONINFOW rovi = { 0 };
  return rovi;
}
#endif

int ctkUtf8Test1(int, char*[])
{

#ifdef _WIN32
  // Check current windows version before proceeding
  RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
  std::cout << "Windows version: " << rovi.dwMajorVersion << "." << rovi.dwMinorVersion << " build " << rovi.dwBuildNumber << std::endl;
  if (rovi.dwBuildNumber < 18362)
  {
    std::cout << "This Windows version does not support UTF-8 as active code page in application (minimum build number is 18362). Further testing is skipped." << std::endl;
    return 0;
  }

  // Check that active code page is UTF-8 on Windows
  UINT activeCodePage = GetACP();
  std::cout << "Active code page: " << activeCodePage << std::endl;
  if (activeCodePage != CP_UTF8)
  {
    std::cerr << "Error: active code page is " << activeCodePage << ", expected " << CP_UTF8 << " (UTF-8)" << std::endl;
    return 1;
  }
#endif

  // Check that we can create a file wit utf8 filename using standard file API
  std::string filenameUtf8 = u8"alpha(\u03b1).txt";
  std::ofstream outputFileStream;
  outputFileStream.open(filenameUtf8);
  if (!outputFileStream.is_open())
  {
    std::cerr << "Failed to open file for writing: " << filenameUtf8.c_str() << std::endl;
    return 1;
  }
  outputFileStream << "some text" << std::endl;
  outputFileStream.close();

  // Test if file is created successfully
  // We use special wide character API on Windows to ensure we properly check for the existence of the file
  // regardless of active code page.
#if _WIN32
  std::wstring filenameW;
  filenameW += L"alpha(";
  filenameW.push_back((wchar_t)(0x03B1));
  filenameW += L").txt";
  FILE* tmp = _wfopen(filenameW.c_str(), L"r");
#else
  FILE* tmp = fopen(filenameUtf8.c_str(), "r");
#endif
  if (!tmp)
  {
    std::cerr << "Expected file does not exist: " << filenameUtf8.c_str() << std::endl;
    return 1;
  }
  fclose(tmp);

  // Delete the temporary file
#if _WIN32
  _wunlink(filenameW.c_str());
#else
  unlink(filenameUtf8.c_str());
#endif

  return 0;
}
