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

#ifndef CTKVERSION_H
#define CTKVERSION_H

#include <QString>
#include <QRegExp>

#include "ctkPluginFrameworkExport.h"


/**
 * \ingroup PluginFramework
 *
 * Version identifier for plug-ins and packages.
 *
 * <p>
 * Version identifiers have four components.
 * <ol>
 * <li>Major version. A non-negative integer.</li>
 * <li>Minor version. A non-negative integer.</li>
 * <li>Micro version. A non-negative integer.</li>
 * <li>Qualifier. A text string. See <code>ctkVersion(const QString&)</code> for the
 * format of the qualifier string.</li>
 * </ol>
 *
 * <p>
 * <code>ctkVersion</code> objects are immutable.
 */
class CTK_PLUGINFW_EXPORT ctkVersion {

private:

  friend class ctkPluginPrivate;
  friend class ctkVersionRange;

  unsigned int majorVersion;
  unsigned int minorVersion;
  unsigned int microVersion;
  QString      qualifier;

  static const QString SEPARATOR; //  = "."
  static const QRegExp RegExp;

  bool undefined;


  /**
   * Called by the ctkVersion constructors to validate the version components.
   *
   * @return <code>true</code> if the validation was successfull, <code>false</code> otherwise.
   */
  void validate();

  ctkVersion& operator=(const ctkVersion& v);

  ctkVersion(bool undefined = false);

public:

  /**
   * The empty version "0.0.0".
   */
  static ctkVersion emptyVersion();

  /**
   * Creates an undefined version identifier, representing either
   * infinity or minus infinity.
   */
  static ctkVersion undefinedVersion();

  /**
   * Creates a version identifier from the specified numerical components.
   *
   * <p>
   * The qualifier is set to the empty string.
   *
   * @param majorVersion Major component of the version identifier.
   * @param minorVersion Minor component of the version identifier.
   * @param microVersion Micro component of the version identifier.
   *
   */
  ctkVersion(unsigned int majorVersion, unsigned int minorVersion, unsigned int microVersion);

  /**
   * Creates a version identifier from the specified components.
   *
   * @param majorVersion Major component of the version identifier.
   * @param minorVersion Minor component of the version identifier.
   * @param microVersion Micro component of the version identifier.
   * @param qualifier Qualifier component of the version identifier.
   */
  ctkVersion(unsigned int majorVersion, unsigned int minorVersion, unsigned int microVersion, const QString& qualifier);

  /**
   * Created a version identifier from the specified string.
   *
   * <p>
   * Here is the grammar for version strings.
   *
   * <pre>
   * version ::= majorVersion('.'minorVersion('.'microVersion('.'qualifier)?)?)?
   * majorVersion ::= digit+
   * minorVersion ::= digit+
   * microVersion ::= digit+
   * qualifier ::= (alpha|digit|'_'|'-')+
   * digit ::= [0..9]
   * alpha ::= [a..zA..Z]
   * </pre>
   *
   * There must be no whitespace in version.
   *
   * @param version string representation of the version identifier.
   */
  ctkVersion(const QString& version);

  /**
   * Create a version identifier from another.
   *
   * @param version Another version identifier
   */
  ctkVersion(const ctkVersion& version);


  /**
   * Parses a version identifier from the specified string.
   *
   * <p>
   * See <code>ctkVersion(const QString&)</code> for the format of the version string.
   *
   * @param version string representation of the version identifier. Leading
   *        and trailing whitespace will be ignored.
   * @return A <code>ctkVersion</code> object representing the version
   *         identifier. If <code>version</code> is the empty string
   *         then <code>emptyVersion</code> will be
   *         returned.
   */
  static ctkVersion parseVersion(const QString& version);

  /**
   * Returns the undefined state of this version identifier.
   *
   * @return <code>true</code> if this version identifier is undefined,
   *         <code>false</code> otherwise.
   */
  bool isUndefined() const;

  /**
   * Returns the majorVersion component of this version identifier.
   *
   * @return The majorVersion component.
   */
  unsigned int getMajor() const;

  /**
   * Returns the minorVersion component of this version identifier.
   *
   * @return The minorVersion component.
   */
  unsigned int getMinor() const;

  /**
   * Returns the microVersion component of this version identifier.
   *
   * @return The microVersion component.
   */
  unsigned int getMicro() const;

  /**
   * Returns the qualifier component of this version identifier.
   *
   * @return The qualifier component.
   */
  QString getQualifier() const;

  /**
   * Returns the string representation of this version identifier.
   *
   * <p>
   * The format of the version string will be <code>majorVersion.minorVersion.microVersion</code>
   * if qualifier is the empty string or
   * <code>majorVersion.minorVersion.microVersion.qualifier</code> otherwise.
   *
   * @return The string representation of this version identifier.
   */
  QString toString() const;

  /**
   * Compares this <code>ctkVersion</code> object to another object.
   *
   * <p>
   * A version is considered to be <b>equal to </b> another version if the
   * majorVersion, minorVersion and microVersion components are equal and the qualifier component
   * is equal.
   *
   * @param object The <code>ctkVersion</code> object to be compared.
   * @return <code>true</code> if <code>object</code> is a
   *         <code>ctkVersion</code> and is equal to this object;
   *         <code>false</code> otherwise.
   */
  bool operator==(const ctkVersion& object) const;

  /**
   * Compares this <code>ctkVersion</code> object to another object.
   *
   * <p>
   * A version is considered to be <b>less than </b> another version if its
   * majorVersion component is less than the other version's majorVersion component, or the
   * majorVersion components are equal and its minorVersion component is less than the other
   * version's minorVersion component, or the majorVersion and minorVersion components are equal
   * and its microVersion component is less than the other version's microVersion component,
   * or the majorVersion, minorVersion and microVersion components are equal and it's qualifier
   * component is less than the other version's qualifier component (using
   * <code>std::string::compare</code>).
   *
   * <p>
   * A version is considered to be <b>equal to</b> another version if the
   * majorVersion, minorVersion and microVersion components are equal and the qualifier component
   * is equal.
   *
   * @param object The <code>ctkVersion</code> object to be compared.
   * @return A negative integer, zero, or a positive integer if this object is
   *         less than, equal to, or greater than the specified
   *         <code>ctkVersion</code> object.
   */
  int compare(const ctkVersion& object) const;

  /**
   * Compares this <code>ctkVersion</code> object to another object.
   *
   * This is a convenience function for use in ordered containers. It relies on
   * the compare(const ctkVersion&) method.
   *
   * @param object The <code>ctkVersion</code> object to be compared.
   * @return <code>true</code> if this object is less then the specified
   *         <code>ctkVersion</code> object, <code>false</code> otherwise.
   */
  bool operator<(const ctkVersion& object) const;

};

/**
 * \ingroup PluginFramework
 */
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkVersion& v);

#endif // CTKVERSION_H
