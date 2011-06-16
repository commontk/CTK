/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#ifndef CTKPLUGINFRAMEWORKTESTUTILEXPORT_H
#define CTKPLUGINFRAMEWORKTESTUTILEXPORT_H

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
#  if defined(CTKPluginFrameworkTestUtil_EXPORTS)
#    define CTK_PLUGINFW_TESTUTIL_EXPORT Q_DECL_EXPORT
#  else
#    define CTK_PLUGINFW_TESTUTIL_EXPORT Q_DECL_IMPORT
#  endif
#endif

#if !defined(CTK_PLUGINFW_TESTUTIL_EXPORT)
//#  if defined(CTK_SHARED)
#    define CTK_PLUGINFW_TESTUTIL_EXPORT Q_DECL_EXPORT
//#  else
//#    define @MY_LIBRARY_EXPORT_DIRECTIVE@
//#  endif
#endif

#endif // CTKPLUGINFRAMEWORKTESTUTILEXPORT_H
