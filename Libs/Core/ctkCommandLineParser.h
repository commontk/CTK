#ifndef __ctkCommandLineParser_h
#define __ctkCommandLineParser_h

// Qt includes
#include <QString>
#include <QStringList>
#include <QVariant>

// CTK includes
#include "CTKCoreExport.h"

// --------------------------------------------------------------------------
class CTK_CORE_EXPORT ctkCommandLineParser
{
public:

  ctkCommandLineParser();
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
   * Returns a detailed error description if a call to <code>parseArguments()</code>
   * failed.
   *
   * @return The error description, empty if no error occured.
   * @see parseArguments(const QStringList&, bool*)
   */
  QString errorString();
  
  /**
   * This method returns all unparsed arguments, i.e. all arguments
   * for which no long or short name has been registered via a call
   * to <code>addArgument()</code>.
   *
   * @see addArgument()
   *
   * @return A list containing unparsed arguments.
   */
  const QStringList& unparsedArguments();
  
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
  bool argumentAdded(const QString& argument);

  /**
   * Checks if the given argument has been parsed successfully by a previous
   * call to <code>parseArguments()</code>.
   *
   * @param argument The argument to be checked.
   * @return <code>true</code> if the argument was parsed, <code>false</code>
   *         otherwise.
   */
  bool argumentParsed(const QString& argument);

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
   * exception is thrown.
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
   *
   * @see setExactMatchRegularExpression()
   * @throws std::logic_error If the QVariant type of <code>defaultValue</code>
   *         does not match <code>type</code>, a <code>std::logic_error</code> is thrown.
   */
  void addArgument(const QString& longarg, const QString& shortarg,
                   QVariant::Type type, const QString& argHelp = QString(),
                   const QVariant& defaultValue = QVariant(), bool ignoreRest = false);

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

  int fieldWidth();

  /**
   * Creates a help text containing properly formatted argument names and help strings
   * provided by calls to <code>addArgument()</code>.
   *
   * @param charPad The padding character.
   * @return The formatted help text.
   */
  QString helpText(const char charPad = ' ');
  
private:
  class ctkInternal;
  ctkInternal * Internal;
};
#endif
