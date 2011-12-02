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

#ifndef __ctkCommandLineParser_h
#define __ctkCommandLineParser_h

// Qt includes
#include <QString>
#include <QStringList>
#include <QVariant>

class QSettings;

// CTK includes
#include "ctkCoreExport.h"

/**
 * \ingroup Core
 *
 * The CTK command line parser.
 *
 * Use this class to add information about the command line arguments
 * your program understands and to easily parse them from a given list
 * of strings.
 *
 * This parser provides the following features:
 *
 * <ul>
 * <li>Add arguments by supplying a long name and/or a short name.
 *     Arguments are validated using a regular expression. They can have
 *     a default value and a help string.</li>
 * <li>Deprecated arguments.</li>
 * <li>Custom regular expressions for argument validation.</li>
 * <li>Set different argument name prefixes for native platform look and feel.</li>
 * <li>QSettings support. Default values for arguments can be read from
 *     a QSettings object.</li>
 * <li>Create a help text for the command line arguments with support for
 *     grouping arguments.</li>
 * </ul>
 *
 * Here is an example how to use this class inside a main function:
 *
 * \snippet CommandLineParser/main.cpp 0
 */
class CTK_CORE_EXPORT ctkCommandLineParser : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString errorString READ errorString)
  Q_PROPERTY(QStringList unparsedArguments READ unparsedArguments)
  Q_PROPERTY(bool settingsEnabled READ settingsEnabled)

public:

  typedef QObject Superclass;

  /**
   * Constructs a parser instance.
   *
   * If QSettings support is enabled by a call to <code>enableSettings()</code>
   * a default constructed QSettings instance will be used when parsing
   * the command line arguments. Make sure to call <code>QCoreApplication::setOrganizationName()</code>
   * and <code>QCoreApplication::setApplicationName()</code> before using default
   * constructed QSettings objects.
   *
   * @param newParent The QObject parent.
   */
  ctkCommandLineParser(QObject* newParent = 0);

  /**
   * Constructs a parser instance.
   *
   * If QSettings support is enabled by a call to <code>enableSettings()</code>
   * the provided QSettings instance will be used. If the QSettings instance is
   * zero, a default constructed QSettings instance will be used when parsing
   * the command line arguments. Using a default constructed instance is usually
   * what you want, if you have called <code>QCoreApplication::setOrganizationName()</code>
   * and <code>QCoreApplication::setApplicationName()</code>.
   *
   * @param settings A QSettings instance which should be used.
   * @param newParent The QObject parent.
   *
   *
   */
  ctkCommandLineParser(QSettings* settings, QObject* newParent = 0);

  ~ctkCommandLineParser();
  
  /**
   * Parse a given list of command line arguments.
   *
   * This method parses a list of QString elements considering the known arguments
   * added by calls to <code>addArgument()</code>. If any one of the argument
   * values does not match the corresponding regular expression,
   * <code>ok</code> is set to false and an empty QHash object is returned.
   *
   * The keys in the returned QHash object correspond to the long argument string,
   * if it is not empty. Otherwise, the short argument string is used as key. The
   * QVariant values can safely be converted to the type specified in the
   * <code>addArgument()</code> method call.
   *
   * @param arguments A QStringList containing command line arguments. Usually
   *        given by <code>QCoreApplication::arguments()</code>.
   * @param ok A pointer to a boolean variable. Will be set to <code>true</code>
   *        if all regular expressions matched, <code>false</code> otherwise.
   * @return A QHash object mapping the long argument (if empty, the short one)
   *         to a QVariant containing the value.
   */
  QHash<QString, QVariant> parseArguments(const QStringList &arguments, bool* ok = 0);

  /**
    * Convenient method allowing to parse a given list of command line arguments.
    * @see parseArguments(const QStringList &, bool*)
    */
  QHash<QString, QVariant> parseArguments(int argc, char** argv, bool* ok = 0);

  /**
   * Returns a detailed error description if a call to <code>parseArguments()</code>
   * failed.
   *
   * @return The error description, empty if no error occured.
   * @see parseArguments(const QStringList&, bool*)
   */
  QString errorString() const;
  
  /**
   * This method returns all unparsed arguments, i.e. all arguments
   * for which no long or short name has been registered via a call
   * to <code>addArgument()</code>.
   *
   * @see addArgument()
   *
   * @return A list containing unparsed arguments.
   */
  const QStringList& unparsedArguments() const;
  
  /**
   * Checks if the given argument has been added via a call
   * to <code>addArgument()</code>.
   *
   * @see addArgument()
   *
   * @param argument The argument to be checked.
   * @return <code>true</code> if the argument was added, <code>false</code>
   *         otherwise.
   */
  Q_INVOKABLE bool argumentAdded(const QString& argument) const;

  /**
   * Checks if the given argument has been parsed successfully by a previous
   * call to <code>parseArguments()</code>.
   *
   * @param argument The argument to be checked.
   * @return <code>true</code> if the argument was parsed, <code>false</code>
   *         otherwise.
   */
  Q_INVOKABLE bool argumentParsed(const QString& argument) const;

  /**
   * Adds a command line argument. An argument can have a long name
   * (like --long-argument-name), a short name (like -l), or both. The type
   * of the argument can be specified by using the <code>type</code> parameter.
   * The following types are supported:
   *
   * <table>
   * <tr><td><b>Type</b></td><td><b># of parameters</b></td><td><b>Default regular expr</b></td>
   *        <td><b>Example</b></td></tr>
   * <tr><td>QVariant::String</td><td>1</td><td>.*</td><td>--test-string StringParameter</td></tr>
   * <tr><td>QVariant::Bool</td><td>0</td><td>does not apply</td><td>--enable-something</td></tr>
   * <tr><td>QVariant::StringList</td><td>-1</td><td>.*</td><td>--test-list string1 string2</td></tr>
   * <tr><td>QVariant::Int</td><td>1</td><td>-?[0-9]+</td><td>--test-int -5</td></tr>
   * </table>
   *
   * The regular expressions are used to validate the parameters of command line
   * arguments. You can restrict the valid set of parameters by calling
   * <code>setExactMatchRegularExpression()</code> for your argument.
   *
   * Optionally, a help string and a default value can be provided for the argument. If
   * the QVariant type of the default value does not match <code>type</code>, an
   * exception is thrown. Arguments with default values are always returned by
   * <code>parseArguments()</code>.
   *
   * You can also declare an argument deprecated, by setting <code>deprecated</code>
   * to <code>true</code>. Alternatively you can add a deprecated argument by calling
   * <code>addDeprecatedArgument()</code>.
   *
   * If the long or short argument has already been added, or if both are empty strings,
   * the method call has no effect.
   *
   * @param longarg The long argument name.
   * @param shortarg The short argument name.
   * @param type The argument type (see the list above for supported types).
   * @param argHelp A help string describing the argument.
   * @param defaultValue A default value for the argument.
   * @param ignoreRest All arguments after the current one will be ignored.
   * @param deprecated Declares the argument deprecated.
   *
   * @see setExactMatchRegularExpression()
   * @see addDeprecatedArgument()
   * @throws std::logic_error If the QVariant type of <code>defaultValue</code>
   *         does not match <code>type</code>, a <code>std::logic_error</code> is thrown.
   */
  void addArgument(const QString& longarg, const QString& shortarg,
                   QVariant::Type type, const QString& argHelp = QString(),
                   const QVariant& defaultValue = QVariant(),
                   bool ignoreRest = false, bool deprecated = false);

  /**
   * Adds a deprecated command line argument. If a deprecated argument is provided
   * on the command line, <code>argHelp</code> is displayed in the console and
   * processing continues with the next argument.
   *
   * Deprecated arguments are grouped separately at the end of the help text
   * returned by <code>helpText()</code>.
   *
   * @param longarg The long argument name.
   * @param shortarg The short argument name.
   * @param argHelp A help string describing alternatives to the deprecated argument.
   */
  void addDeprecatedArgument(const QString& longarg, const QString& shortarg,
                             const QString& argHelp);

  /**
   * Sets a custom regular expression for validating argument parameters. The method
   * <code>errorString()</code> can be used the get the last error description.
   *
   * @param argument The previously added long or short argument name.
   * @param expression A regular expression which the arugment parameters must match.
   * @param exactMatchFailedMessage An error message explaining why the parameter did
   *        not match.
   *
   * @return <code>true</code> if the argument was found and the regular expression was set,
   *         <code>false</code> otherwise.
   *
   * @see errorString()
   */
  bool setExactMatchRegularExpression(const QString& argument, const QString& expression,
                                      const QString& exactMatchFailedMessage);

  /**
   * The field width for the argument names without the help text.
   *
   * @return The argument names field width in the help text.
   */
  int fieldWidth() const;

  /**
   * Creates a help text containing properly formatted argument names and help strings
   * provided by calls to <code>addArgument()</code>. The arguments can be grouped by
   * using <code>beginGroup()</code> and <code>endGroup()</code>.
   *
   * @param charPad The padding character.
   * @return The formatted help text.
   */
  QString helpText(const char charPad = ' ') const;

  /**
   * Sets the argument prefix for long and short argument names. This can be used
   * to create native command line arguments without changing the calls to
   * <code>addArgument()</code>. For example on Unix-based systems, long argument
   * names start with "--" and short names with "-", while on Windows argument names
   * always start with "/".
   *
   * Note that all methods in ctkCommandLineParser which take an argument name
   * expect the name as it was supplied to <code>addArgument</code>.
   *
   * Example usage:
   *
   * \code
   * ctkCommandLineParser parser;
   * parser.setArgumentPrefix("--", "-");
   * parser.addArgument("long-argument", "l", QVariant::String);
   * QStringList args;
   * args << "program name" << "--long-argument Hi";
   * parser.parseArguments(args);
   * \endcode
   *
   * @param longPrefix The prefix for long argument names.
   * @param shortPrefix The prefix for short argument names.
   */
  void setArgumentPrefix(const QString& longPrefix, const QString& shortPrefix);

  /**
   * Begins a new group for documenting arguments. All newly added arguments via
   * <code>addArgument()</code> will be put in the new group. You can close the
   * current group by calling <code>endGroup()</code> or be opening a new group.
   *
   * Note that groups cannot be nested and all arguments which do not belong to
   * a group will be listed at the top of the text created by <code>helpText()</code>.
   *
   * @param description The description of the group
   */
  void beginGroup(const QString& description);

  /**
   * Ends the current group.
   *
   * @see beginGroup(const QString&)
   */
  void endGroup();

  /**
   * Enables QSettings support in ctkCommandLineParser. If an argument name is found
   * in the QSettings instance with a valid QVariant, the value is considered as
   * a default value and overwrites default values registered with
   * <code>addArgument()</code>. User supplied values on the command line overwrite
   * values in the QSettings instance, except for arguments with multiple parameters
   * which are merged with QSettings values. Call <code>mergeSettings(false)</code>
   * to disable merging.
   *
   * See <code>ctkCommandLineParser(QSettings*)</code> for information about how to
   * supply a QSettings instance.
   *
   * Additionally, a long and short argument name can be specified which will disable
   * QSettings support if supplied on the command line. The argument name must be
   * registered as a regular argument via <code>addArgument()</code>.
   *
   * @param disableLongArg Long argument name.
   * @param disableShortArg Short argument name.
   *
   * @see ctkCommandLineParser(QSettings*)
   */
  void enableSettings(const QString& disableLongArg = "",
                      const QString& disableShortArg = "");

  /**
   * Controlls the merging behavior of user values and QSettings values.
   *
   * If merging is on (the default), user supplied values for an argument
   * which can take more than one parameter are merged with values stored
   * in the QSettings instance. If merging is off, the user values overwrite
   * the QSettings values.
   *
   * @param merge <code>true</code> enables QSettings merging, <code>false</code>
   *        disables it.
   */
  void mergeSettings(bool merge);

  /**
   * Can be used to check if QSettings support has been enabled by a call to
   * <code>enableSettings()</code>.
   *
   * @return <code>true</code> if QSettings support is enabled, <code>false</code>
   *         otherwise.
   */
  bool settingsEnabled() const;


  /**
    * Can be used to teach the parser to stop parsing the arguments and return False when
    * an unknown argument is encountered. By default <code>StrictMode</code> is disabled.
    *
    * @see parseArguments(const QStringList &, bool*)
    */
  void setStrictModeEnabled(bool strictMode);

private:
  class ctkInternal;
  ctkInternal * Internal;
};
#endif
