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


#ifndef CTKCASEINSENSITIVESTRING_P_H
#define CTKCASEINSENSITIVESTRING_P_H

#include <QString>

#include <ctkPluginFrameworkExport.h>

/**
 * ctkCaseInsensitiveString wraps a QString and can be
 * used in Qt container classes as a key type representing
 * case insensitive strings. However, case is preserved when
 * retrieving the actual QString.
 */
class CTK_PLUGINFW_EXPORT ctkCaseInsensitiveString
{

public:

  /**
   * Wraps a null QString.
   */
  ctkCaseInsensitiveString();

  /**
   * Wraps the given character sequence.
   *
   * @param str The characters to be wrapped by this ctkCaseInsensitiveString
   */
  ctkCaseInsensitiveString(const char* str);

  /**
   * Wraps the given QString.
   *
   * @param str The QString to be wrapped by this ctkCaseInsensitiveString
   */
  ctkCaseInsensitiveString(const QString& str);

  /**
   * Copy constructor.
   *
   * @param str The ctkCaseInsensitiveString instance to copy.
   */
  ctkCaseInsensitiveString(const ctkCaseInsensitiveString& str);

  /**
   * Assignment operator.
   *
   * @param str The ctkCaseInsensitiveString instance which should be
   *        assigned.
   */
  ctkCaseInsensitiveString& operator=(const ctkCaseInsensitiveString& str);

  /**
   * String comparison ignoring case.
   *
   * @param str The string with which to compare this instance.
   * @return <code>true</code> if both strings are equal after being
   *         converted to lower case strings, <code>false</code> otherwise.
   */
  bool operator==(const ctkCaseInsensitiveString& str) const;

  /**
   * Less than operator ignoring case.
   *
   * @param str The string with which to compare this instance.
   * @return <code>true</code> if the lower case variant of the
   *         current string is lexicographically less then
   *         the lower case variant of <code>str</code>, <code>false</code>
   *         otherwise.
   */
  bool operator<(const ctkCaseInsensitiveString& str) const;

  /**
   * Converts this ctkCaseInsensitiveString instance to a QString,
   * preserving the original case.
   */
  operator QString() const;

private:

  QString str;
};

/**
 * Returns a hash value for the lower case string.
 *
 * @param str The string to be hashed.
 */
uint CTK_PLUGINFW_EXPORT qHash(const ctkCaseInsensitiveString& str);

CTK_PLUGINFW_EXPORT QDataStream& operator<<(QDataStream &out, const ctkCaseInsensitiveString& str);
CTK_PLUGINFW_EXPORT QDataStream& operator>>(QDataStream &in, ctkCaseInsensitiveString& str);

#endif // CTKCASEINSENSITIVESTRING_P_H
