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

#ifndef __ctkWidgetsTestingUtilities_h
#define __ctkWidgetsTestingUtilities_h

// CTK includes
#include <ctkWidgetsExport.h>

class QImage;

/// This module provides functions to facilitate writing tests.

namespace ctkWidgetsTestingUtilities
{

/// Compare two images for pixel-level similarity. Allow for the specified
/// percentage of pixels to be different.
CTK_WIDGETS_EXPORT
bool CompareImages(const QImage& current, const QImage& expected,
                   float percentThreshold=0.f);

} // namespace ctkWidgetsTestingUtilities

#endif
