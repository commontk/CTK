#ifndef CTKVERSION_H
#define CTKVERSION_H

#include <QString>
#include <QRegExp>

namespace ctk {

  /**
   * Version identifier for plug-ins and packages.
   *
   * <p>
   * Version identifiers have four components.
   * <ol>
   * <li>Major version. A non-negative integer.</li>
   * <li>Minor version. A non-negative integer.</li>
   * <li>Micro version. A non-negative integer.</li>
   * <li>Qualifier. A text string. See <code>Version(const QString&)</code> for the
   * format of the qualifier string.</li>
   * </ol>
   *
   * <p>
   * <code>Version</code> objects are immutable.
   *
   * @Immutable
   */

  class Version {

  private:

    bool valid;

    unsigned int major;
    unsigned int minor;
    unsigned int micro;
    QString      qualifier;

    static const QString SEPARATOR; //  = "."
    static const QRegExp RegExp;


    /**
     * Called by the Version constructors to validate the version components.
     *
     * @return <code>true</code> if the validation was successfull, <code>false</code> otherwise.
     */
    void validate();

  public:

    /**
     * The empty version "0.0.0".
     */
    static const Version emptyVersion;

    /**
     * Creates a version identifier from the specified numerical components.
     *
     * <p>
     * The qualifier is set to the empty string.
     *
     * @param major Major component of the version identifier.
     * @param minor Minor component of the version identifier.
     * @param micro Micro component of the version identifier.
     *
     */
    Version(unsigned int major, unsigned int minor, unsigned int micro);

    /**
     * Creates a version identifier from the specified components.
     *
     * @param major Major component of the version identifier.
     * @param minor Minor component of the version identifier.
     * @param micro Micro component of the version identifier.
     * @param qualifier Qualifier component of the version identifier.
     */
    Version(unsigned int major, unsigned int minor, unsigned int micro, const QString& qualifier);

    /**
     * Created a version identifier from the specified string.
     *
     * <p>
     * Here is the grammar for version strings.
     *
     * <pre>
     * version ::= major('.'minor('.'micro('.'qualifier)?)?)?
     * major ::= digit+
     * minor ::= digit+
     * micro ::= digit+
     * qualifier ::= (alpha|digit|'_'|'-')+
     * digit ::= [0..9]
     * alpha ::= [a..zA..Z]
     * </pre>
     *
     * There must be no whitespace in version.
     *
     * @param version string representation of the version identifier.
     */
    Version(const QString& version);

    /**
     * Create a version identifier from another.
     *
     * @param version Another version identifier
     */
    Version(const Version& version);


    /**
     * Parses a version identifier from the specified string.
     *
     * <p>
     * See <code>Version(const QString&)</code> for the format of the version string.
     *
     * @param version string representation of the version identifier. Leading
     *        and trailing whitespace will be ignored.
     * @return A <code>Version</code> object representing the version
     *         identifier. If <code>version</code> is the empty string
     *         then <code>emptyVersion</code> will be
     *         returned.
     */
    static Version parseVersion(const QString& version);

    /**
     * Returns if the version is valid.
     */
    bool isValid() const;

    /**
     * Returns the major component of this version identifier.
     *
     * @return The major component.
     */
    unsigned int getMajor() const;

    /**
     * Returns the minor component of this version identifier.
     *
     * @return The minor component.
     */
    unsigned int getMinor() const;

    /**
     * Returns the micro component of this version identifier.
     *
     * @return The micro component.
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
     * The format of the version string will be <code>major.minor.micro</code>
     * if qualifier is the empty string or
     * <code>major.minor.micro.qualifier</code> otherwise.
     *
     * @return The string representation of this version identifier.
     */
    QString toString() const;

    /**
     * Compares this <code>Version</code> object to another object.
     *
     * <p>
     * A version is considered to be <b>equal to </b> another version if the
     * major, minor and micro components are equal and the qualifier component
     * is equal.
     *
     * @param object The <code>Version</code> object to be compared.
     * @return <code>true</code> if <code>object</code> is a
     *         <code>Version</code> and is equal to this object;
     *         <code>false</code> otherwise.
     */
    bool operator==(const Version& object) const;

    /**
     * Compares this <code>Version</code> object to another object.
     *
     * <p>
     * A version is considered to be <b>less than </b> another version if its
     * major component is less than the other version's major component, or the
     * major components are equal and its minor component is less than the other
     * version's minor component, or the major and minor components are equal
     * and its micro component is less than the other version's micro component,
     * or the major, minor and micro components are equal and it's qualifier
     * component is less than the other version's qualifier component (using
     * <code>std::string::compare</code>).
     *
     * <p>
     * A version is considered to be <b>equal to</b> another version if the
     * major, minor and micro components are equal and the qualifier component
     * is equal.
     *
     * @param object The <code>Version</code> object to be compared.
     * @return A negative integer, zero, or a positive integer if this object is
     *         less than, equal to, or greater than the specified
     *         <code>Version</code> object.
     */
    int compare(const Version& object) const;

  };

}

#endif // CTKVERSION_H
