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

#ifndef __ctkDependencyGraphTestHelper_h
#define __ctkDependencyGraphTestHelper_h

#include <list>
#include <iostream>

namespace
{

void printIntegerList(const char* msg, const std::list<int>& list, bool endl = true)
{
  std::cerr << msg;
  std::list<int>::const_iterator iter;
  for (iter = list.begin(); iter != list.end(); iter++)
    {
    std::cerr << *iter << " ";
    }
  if (endl)
    {
    std::cerr << std::endl;
    }
}

}

#endif // end __ctkDependencyGraphTestHelper_h
