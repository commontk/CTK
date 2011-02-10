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

#ifndef CTKVERSIONRANGE_H
#define CTKVERSIONRANGE_H

#include "ctkVersion.h"


/**
 * \ingroup PluginFramework
 *
 * Class representing CTK version ranges.
 */
class ctkVersionRange
{

private:

  ctkVersion low;
  ctkVersion high;
  bool lowIncluded;
  bool highIncluded;

public:

  /**
   * The empty version range "[0.0.0,inf)".
   */
  static ctkVersionRange defaultVersionRange();

  /**
   * Construct a ctkVersionRange object.
   * Format for a range:
   *   ( "(" | "[" ) LOW_VERSION ","  HIGH_VERSION ( ")" | "]" )
   * Format for at least a version:
   *   VERSION
   *
   * @param vr Input string.
   */
  ctkVersionRange(const QString& vr);


  /**
   * Construct the default ctkVersionRange object.
   *
   */
  ctkVersionRange();

  ~ctkVersionRange();


  bool isSpecified() const;


  /**
   * Check if specified version is within our range.
   *
   * @param ver ctkVersion to compare to.
   * @return Return true if within range, otherwise false.
   */
  bool withinRange(const ctkVersion& ver) const;


  /**
   * Check if objects range is within another ctkVersionRange.
   *
   * @param range ctkVersionRange to compare to.
   * @return Return true if within range, otherwise false.
   */
  bool withinRange(const ctkVersionRange& range) const;


  /**
   * Compare object to another ctkVersionRange. VersionRanges are compared on the
   * lower bound.
   *
   * @param obj ctkVersionRange to compare to.
   * @return Return 0 if equals, negative if this object is less than obj
   *         and positive if this object is larger then obj.
   * @exception ClassCastException if object is not a ctkVersionRange object.
   */
  int compare(const ctkVersionRange& obj) const;


  /**
   * String with version number. If version is not specified return
   * an empty string.
   *
   * @return QString.
   */
  QString toString() const;


  /**
   * Check if object is equal to this object.
   *
   * @param obj Package entry to compare to.
   * @return true if equal, otherwise false.
   */
  bool operator==(const ctkVersionRange& r) const;

};


#endif // CTKVERSIONRANGE_H
