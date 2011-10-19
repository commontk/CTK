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


#ifndef CTKTESTPLUGINSSERVICE0_H
#define CTKTESTPLUGINSSERVICE0_H

#include <qglobal.h>

struct ctkTestPluginSService0
{
  virtual ~ctkTestPluginSService0() {}
};

Q_DECLARE_INTERFACE(ctkTestPluginSService0, "org.commontk.pluginStest.TestPluginSService0")

#endif // CTKTESTPLUGINSSERVICE0_H
