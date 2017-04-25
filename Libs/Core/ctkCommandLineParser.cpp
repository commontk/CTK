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

// STL includes
#include <stdexcept>

// Qt includes 
#include <QHash>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QPointer>

// CTK includes
#include "ctkCommandLineParser.h"

namespace
{
// --------------------------------------------------------------------------
class CommandLineParserArgumentDescription
{
public:
  CommandLineParserArgumentDescription(
    const QString& longArg, const QString& longArgPrefix,
    const QString& shortArg, const QString& shortArgPrefix,
    QVariant::Type type, const QString& argHelp,
    const QVariant& defaultValue, bool ignoreRest,
    bool deprecated)
      : LongArg(longArg), LongArgPrefix(longArgPrefix),
      ShortArg(shortArg), ShortArgPrefix(shortArgPrefix),
      ArgHelp(argHelp), IgnoreRest(ignoreRest), NumberOfParametersToProcess(0),
      Deprecated(deprecated), DefaultValue(defaultValue), Value(type), ValueType(type)
  {
    if (defaultValue.isValid())
      {
      Value = defaultValue;
      }

    switch (type)
      {
      case QVariant::String:
        {
        NumberOfParametersToProcess = 1;
        RegularExpression = ".*";
        }
        break;
      case QVariant::Bool:
        {
        NumberOfParametersToProcess = 0;
        RegularExpression = "";
        }
        break;
      case QVariant::StringList:
        {
        NumberOfParametersToProcess = -1;
        RegularExpression = ".*";
        }
        break;
      case QVariant::Int:
        {
        NumberOfParametersToProcess = 1;
        RegularExpression = "-?[0-9]+";
        ExactMatchFailedMessage = "A negative or positive integer is expected.";
        }
        break;
      case QVariant::Double:
        {
        NumberOfParametersToProcess = 1;
        RegularExpression = "-?[0-9]*\\.?[0-9]+";
        ExactMatchFailedMessage = "A double is expected.";
        }
        break;
      default:
        ExactMatchFailedMessage = QString("Type %1 not supported.").arg(static_cast<int>(type));
      }

  }

  ~CommandLineParserArgumentDescription(){}

  bool addParameter(const QString& value);

  QString helpText(int fieldWidth, const char charPad, const QString& settingsValue = "");

  QString LongArg;
  QString LongArgPrefix;
  QString ShortArg;
  QString ShortArgPrefix;
  QString ArgHelp;
  bool    IgnoreRest;
  int     NumberOfParametersToProcess;
  QString RegularExpression;
  QString ExactMatchFailedMessage;
  bool    Deprecated;

  QVariant       DefaultValue;
  QVariant       Value;
  QVariant::Type ValueType;
};

// --------------------------------------------------------------------------
bool CommandLineParserArgumentDescription::addParameter(const QString& value)
{
  if (!RegularExpression.isEmpty())
    {
    // Validate value
    QRegExp regexp(this->RegularExpression);
    if (!regexp.exactMatch(value))
      {
      return false;
      }
    }

  switch (Value.type())
    {
    case QVariant::String:
      {
      Value.setValue(value);
      }
      break;
    case QVariant::Bool:
      {
      Value.setValue(!QString::compare(value, "true", Qt::CaseInsensitive));
      }
      break;
    case QVariant::StringList:
      {
      if (Value.isNull())
        {
        QStringList list;
        list << value;
        Value.setValue(list);
        }
      else
        {
        QStringList list = Value.toStringList();
        list << value;
        Value.setValue(list);
        }
      }
      break;
    case QVariant::Int:
      {
      Value.setValue(value.toInt());
      }
      break;
    case QVariant::Double:
      {
      Value.setValue(value.toDouble());
      }
      break;
    default:
      return false;
    }

  return true;
}

// --------------------------------------------------------------------------
QString CommandLineParserArgumentDescription::helpText(int fieldWidth, const char charPad,
                                                       const QString& settingsValue)
{
  QString text;
  QTextStream stream(&text);
  stream.setFieldAlignment(QTextStream::AlignLeft);
  stream.setPadChar(charPad);

  QString shortAndLongArg;
  if (!this->ShortArg.isEmpty())
    {
    shortAndLongArg += QString("  %1%2").arg(this->ShortArgPrefix).arg(this->ShortArg);
    }

  if (!this->LongArg.isEmpty())
    {
    if (this->ShortArg.isEmpty())
      {
      shortAndLongArg.append("  ");
      }
    else
      {
      shortAndLongArg.append(", ");
      }

    shortAndLongArg += QString("%1%2").arg(this->LongArgPrefix).arg(this->LongArg);
    }

  if(!this->ArgHelp.isEmpty())
    {
    stream.setFieldWidth(fieldWidth);
    }

  stream  << shortAndLongArg;
  stream.setFieldWidth(0);
  stream << this->ArgHelp;
  if (!settingsValue.isNull())
    {
    stream << " (default: " << settingsValue << ")";
    }
  else if (!this->DefaultValue.isNull())
    {
    stream << " (default: " << this->DefaultValue.toString() << ")";
    }
  stream << "\n";
  return text;
}

}

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal class

// --------------------------------------------------------------------------
class ctkCommandLineParser::ctkInternal
{
public:
  ctkInternal(QSettings* settings)
    : Debug(false), FieldWidth(0), UseQSettings(false),
      Settings(settings), MergeSettings(true), StrictMode(false)
  {}

  ~ctkInternal() { qDeleteAll(ArgumentDescriptionList); }
  
  CommandLineParserArgumentDescription* argumentDescription(const QString& argument);
  
  QList<CommandLineParserArgumentDescription*>                 ArgumentDescriptionList;
  QHash<QString, CommandLineParserArgumentDescription*>        ArgNameToArgumentDescriptionMap;
  QMap<QString, QList<CommandLineParserArgumentDescription*> > GroupToArgumentDescriptionListMap;
  
  QStringList UnparsedArguments; 
  QStringList ProcessedArguments;
  QString     ErrorString;
  bool        Debug;
  int         FieldWidth;
  QString     LongPrefix;
  QString     ShortPrefix;
  QString     CurrentGroup;
  bool        UseQSettings;
  QPointer<QSettings> Settings;
  QString     DisableQSettingsLongArg;
  QString     DisableQSettingsShortArg;
  bool        MergeSettings;
  bool        StrictMode;
};

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal methods

// --------------------------------------------------------------------------
CommandLineParserArgumentDescription*
  ctkCommandLineParser::ctkInternal::argumentDescription(const QString& argument)
{
  QString unprefixedArg = argument;
  if (!LongPrefix.isEmpty() && argument.startsWith(LongPrefix))
    {
    // Case when (ShortPrefix + UnPrefixedArgument) matches LongPrefix
    if (argument == LongPrefix && !ShortPrefix.isEmpty() && argument.startsWith(ShortPrefix))
      {
      unprefixedArg = argument.mid(ShortPrefix.length());
      }
    else
      {
      unprefixedArg = argument.mid(LongPrefix.length());
      }
    }
  else if (!ShortPrefix.isEmpty() && argument.startsWith(ShortPrefix))
    {
    unprefixedArg = argument.mid(ShortPrefix.length());
    }
  else if (!LongPrefix.isEmpty() && !ShortPrefix.isEmpty())
    {
    return 0;
    }

  if (this->ArgNameToArgumentDescriptionMap.contains(unprefixedArg))
    {
    return this->ArgNameToArgumentDescriptionMap[unprefixedArg];
    }
  return 0;
}

// --------------------------------------------------------------------------
// ctkCommandLineParser methods

// --------------------------------------------------------------------------
ctkCommandLineParser::ctkCommandLineParser(QObject* newParent) : Superclass(newParent)
{
  this->Internal = new ctkInternal(0);
}

// --------------------------------------------------------------------------
ctkCommandLineParser::ctkCommandLineParser(QSettings* settings, QObject* newParent) :
    Superclass(newParent)
{
  this->Internal = new ctkInternal(settings);
}

// --------------------------------------------------------------------------
ctkCommandLineParser::~ctkCommandLineParser()
{
  delete this->Internal;
}

// --------------------------------------------------------------------------
QHash<QString, QVariant> ctkCommandLineParser::parseArguments(const QStringList& arguments,
                                                              bool* ok)
{
  // Reset
  this->Internal->UnparsedArguments.clear();
  this->Internal->ProcessedArguments.clear();
  this->Internal->ErrorString.clear();
  foreach (CommandLineParserArgumentDescription* desc,
           this->Internal->ArgumentDescriptionList)
    {
    desc->Value = QVariant(desc->ValueType);
    if (desc->DefaultValue.isValid())
      {
      desc->Value = desc->DefaultValue;
      }
    }

  bool error = false;
  bool ignoreRest = false;
  bool useSettings = this->Internal->UseQSettings;
  CommandLineParserArgumentDescription * currentArgDesc = 0;
  QList<CommandLineParserArgumentDescription*> parsedArgDescriptions;
  for(int i = 1; i < arguments.size(); ++i)
    {
    QString argument = arguments.at(i);
    if (this->Internal->Debug) { qDebug() << "Processing" << argument; }

    // should argument be ignored ?
    if (ignoreRest)
      {
      if (this->Internal->Debug)
        {
        qDebug() << "  Skipping: IgnoreRest flag was been set";
        }
      this->Internal->UnparsedArguments << argument;
      continue;
      }

    // Skip if the argument does not start with the defined prefix
    if (!(argument.startsWith(this->Internal->LongPrefix)
      || argument.startsWith(this->Internal->ShortPrefix)))
      {
      if (this->Internal->StrictMode)
        {
        this->Internal->ErrorString = QString("Unknown argument %1").arg(argument);
        error = true;
        break;
        }
      if (this->Internal->Debug)
        {
        qDebug() << "  Skipping: It does not start with the defined prefix";
        }
      this->Internal->UnparsedArguments << argument;
      continue;
      }

    // Skip if argument has already been parsed ...
    if (this->Internal->ProcessedArguments.contains(argument))
      {
      if (this->Internal->StrictMode)
        {
        this->Internal->ErrorString = QString("Argument %1 already processed !").arg(argument);
        error = true;
        break;
        }
      if (this->Internal->Debug)
        {
        qDebug() << "  Skipping: Already processed !";
        }
      continue;
      }

    // Retrieve corresponding argument description
    currentArgDesc = this->Internal->argumentDescription(argument);

    // Is there a corresponding argument description ?
    if (currentArgDesc)
      {
      // If the argument is deprecated, print the help text but continue processing
      if (currentArgDesc->Deprecated)
        {
        qWarning().nospace() << "Deprecated argument " << argument << ": "  << currentArgDesc->ArgHelp;
        }
      else
        {
        parsedArgDescriptions.push_back(currentArgDesc);
        }

      // Is the argument the special "disable QSettings" argument?
      if ((!currentArgDesc->LongArg.isEmpty() && currentArgDesc->LongArg == this->Internal->DisableQSettingsLongArg)
        || (!currentArgDesc->ShortArg.isEmpty() && currentArgDesc->ShortArg == this->Internal->DisableQSettingsShortArg))
        {
        useSettings = false;
        }

      this->Internal->ProcessedArguments << currentArgDesc->ShortArg << currentArgDesc->LongArg;
      int numberOfParametersToProcess = currentArgDesc->NumberOfParametersToProcess;
      ignoreRest = currentArgDesc->IgnoreRest;
      if (this->Internal->Debug && ignoreRest)
        {
        qDebug() << "  IgnoreRest flag is True";
        }

      // Is the number of parameters associated with the argument being processed known ?
      if (numberOfParametersToProcess == 0)
        {
        currentArgDesc->addParameter("true");
        }
      else if (numberOfParametersToProcess > 0)
        {
        QString missingParameterError =
            "Argument %1 has %2 value(s) associated whereas exacly %3 are expected.";
        for(int j=1; j <= numberOfParametersToProcess; ++j)
          {
          if (i + j >= arguments.size())
            {
            this->Internal->ErrorString = 
                missingParameterError.arg(argument).arg(j-1).arg(numberOfParametersToProcess);
            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            if (ok) { *ok = false; }
            return QHash<QString, QVariant>();
            }
          QString parameter = arguments.at(i + j);
          if (this->Internal->Debug)
            {
            qDebug() << "  Processing parameter" << j << ", value:" << parameter;
            }
          if (this->Internal->argumentDescription(parameter) != 0)
            {
            // we've found a known argument, it means there are no more
            // parameter for the current argument
            this->Internal->ErrorString =
                missingParameterError.arg(argument).arg(j-1).arg(numberOfParametersToProcess);
            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            if (ok) { *ok = false; }
            return QHash<QString, QVariant>();
            }
          if (!currentArgDesc->addParameter(parameter))
            {
            this->Internal->ErrorString = QString(
                "Value(s) associated with argument %1 are incorrect. %2").
                arg(argument).arg(currentArgDesc->ExactMatchFailedMessage);

            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            if (ok) { *ok = false; }
            return QHash<QString, QVariant>();
            }
          }
        // Update main loop increment
        i = i + numberOfParametersToProcess;
        }
      else if (numberOfParametersToProcess == -1)
        {
        if (this->Internal->Debug)
          {
          qDebug() << "  Proccessing StringList ...";
          }
        int j = 1;
        while(j + i < arguments.size())
          {
          if (this->Internal->argumentDescription(arguments.at(j + i)) != 0)
            {
            // we've found a known argument, it means there are no more
            // parameter for the current argument
            if (this->Internal->Debug)
              {
              qDebug() << "  No more parameter for" << argument;
              }
            j--; // this parameter does not belong to current argument
            break;
            }
          QString parameter = arguments.at(j + i);
          if (this->Internal->Debug)
            {
            qDebug() << "  Processing parameter" << j << ", value:" << parameter;
            }
          if (!currentArgDesc->addParameter(parameter))
            {
            this->Internal->ErrorString = QString(
                "Value(s) associated with argument %1 are incorrect. %2").
                arg(argument).arg(currentArgDesc->ExactMatchFailedMessage);

            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            if (ok) { *ok = false; }
            return QHash<QString, QVariant>();
            }
          j++;
          }
        // Update main loop increment
        i = i + j;
        }
      }
    else
      {
      if (this->Internal->StrictMode)
        {
        this->Internal->ErrorString = QString("Unknown argument %1").arg(argument);
        error = true;
        break;
        }
      if (this->Internal->Debug)
        {
        qDebug() << "  Skipping: Unknown argument";
        }
      this->Internal->UnparsedArguments << argument;
      }
    }

  if (ok)
    {
    *ok = !error;
    }

  QSettings* settings = 0;
  if (this->Internal->UseQSettings && useSettings)
    {
    if (this->Internal->Settings)
      {
      settings = this->Internal->Settings;
      }
    else
      {
      // Use a default constructed QSettings instance
      settings = new QSettings();
      }
    }

  QHash<QString, QVariant> parsedArguments;
  QListIterator<CommandLineParserArgumentDescription*> it(this->Internal->ArgumentDescriptionList);
  while (it.hasNext())
    {
    QString key;
    CommandLineParserArgumentDescription* desc = it.next();
    if (!desc->LongArg.isEmpty())
      {
      key = desc->LongArg;
      }
    else
      {
      key = desc->ShortArg;
      }

    if (parsedArgDescriptions.contains(desc))
      {
      // The argument was supplied on the command line, so use the given value

      if (this->Internal->MergeSettings && settings)
        {
        // Merge with QSettings
        QVariant settingsVal = settings->value(key);

        if (desc->ValueType == QVariant::StringList &&
            settingsVal.canConvert(QVariant::StringList))
          {
          QStringList stringList = desc->Value.toStringList();
          stringList.append(settingsVal.toStringList());
          parsedArguments.insert(key, stringList);
          }
        else
          {
          // do a normal insert
          parsedArguments.insert(key, desc->Value);
          }
        }
      else
        {
        // No merging, just insert all user values
        parsedArguments.insert(key, desc->Value);
        }
      }
    else
      {
      if (settings)
        {
        // If there is a valid QSettings entry for the argument, use the value
        QVariant settingsVal = settings->value(key, desc->Value);
        if (!settingsVal.isNull())
          {
          parsedArguments.insert(key, settingsVal);
          }
        }
      else
        {
        // Just insert the arguments with valid default values
        if (!desc->Value.isNull())
          {
          parsedArguments.insert(key, desc->Value);
          }
        }
      }
    }

  // If we created a default QSettings instance, delete it
  if (settings && !this->Internal->Settings)
    {
    delete settings;
    }

  return parsedArguments;
}

// -------------------------------------------------------------------------
QHash<QString, QVariant> ctkCommandLineParser::parseArguments(int argc, char** argv, bool* ok)
{
  QStringList arguments;

  // Create a QStringList of arguments
  for(int i = 0; i < argc; ++i)
    {
    arguments << argv[i];
    }

  return this->parseArguments(arguments, ok);
}

// -------------------------------------------------------------------------
QString ctkCommandLineParser::errorString() const
{
  return this->Internal->ErrorString;
}

// -------------------------------------------------------------------------
const QStringList& ctkCommandLineParser::unparsedArguments() const
{
  return this->Internal->UnparsedArguments;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::addArgument(const QString& longarg, const QString& shortarg,
                                       QVariant::Type type, const QString& argHelp,
                                       const QVariant& defaultValue, bool ignoreRest,
                                       bool deprecated)
{
  Q_ASSERT_X(!(longarg.isEmpty() && shortarg.isEmpty()), "addArgument",
             "both long and short argument names are empty");
  if (longarg.isEmpty() && shortarg.isEmpty()) { return; }

  Q_ASSERT_X(!defaultValue.isValid() || defaultValue.type() == type, "addArgument",
             "defaultValue type does not match");
  if (defaultValue.isValid() && defaultValue.type() != type)
    throw std::logic_error("The QVariant type of defaultValue does not match the specified type");

  /* Make sure it's not already added */
  bool added = this->Internal->ArgNameToArgumentDescriptionMap.contains(longarg);
  Q_ASSERT_X(!added, "addArgument", "long argument already added");
  if (added) { return; }

  added = this->Internal->ArgNameToArgumentDescriptionMap.contains(shortarg);
  Q_ASSERT_X(!added, "addArgument", "short argument already added");
  if (added) { return; }

  CommandLineParserArgumentDescription* argDesc =
    new CommandLineParserArgumentDescription(longarg, this->Internal->LongPrefix,
                                             shortarg, this->Internal->ShortPrefix, type,
                                             argHelp, defaultValue, ignoreRest, deprecated);

  int argWidth = 0;
  if (!longarg.isEmpty())
    {
    this->Internal->ArgNameToArgumentDescriptionMap[longarg] = argDesc;
    argWidth += longarg.length() + this->Internal->LongPrefix.length();
    }
  if (!shortarg.isEmpty())
    {
    this->Internal->ArgNameToArgumentDescriptionMap[shortarg] = argDesc;
    argWidth += shortarg.length() + this->Internal->ShortPrefix.length() + 2;
    }
  argWidth += 5;

  // Set the field width for the arguments
  if (argWidth > this->Internal->FieldWidth)
    {
    this->Internal->FieldWidth = argWidth;
    }

  this->Internal->ArgumentDescriptionList << argDesc;
  this->Internal->GroupToArgumentDescriptionListMap[this->Internal->CurrentGroup] << argDesc;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::addDeprecatedArgument(
    const QString& longarg, const QString& shortarg, const QString& argHelp)
{
  addArgument(longarg, shortarg, QVariant::StringList, argHelp, QVariant(), false, true);
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::setExactMatchRegularExpression(
    const QString& argument, const QString& expression, const QString& exactMatchFailedMessage)
{
  CommandLineParserArgumentDescription * argDesc =
      this->Internal->argumentDescription(argument);
  if (!argDesc)
    {
    return false;
    }

  if (argDesc->Value.type() == QVariant::Bool)
    {
    return false;
    }
  argDesc->RegularExpression = expression;
  argDesc->ExactMatchFailedMessage = exactMatchFailedMessage;
  return true;
}

// --------------------------------------------------------------------------
int ctkCommandLineParser::fieldWidth() const
{
  return this->Internal->FieldWidth;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::beginGroup(const QString& description)
{
  this->Internal->CurrentGroup = description;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::endGroup()
{
  this->Internal->CurrentGroup.clear();
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::enableSettings(const QString& disableLongArg, const QString& disableShortArg)
{
  this->Internal->UseQSettings = true;
  this->Internal->DisableQSettingsLongArg = disableLongArg;
  this->Internal->DisableQSettingsShortArg = disableShortArg;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::mergeSettings(bool merge)
{
  this->Internal->MergeSettings = merge;
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::settingsEnabled() const
{
  return this->Internal->UseQSettings;
}

// --------------------------------------------------------------------------
QString ctkCommandLineParser::helpText(const char charPad) const
{
  QString text;
  QTextStream stream(&text);

  QList<CommandLineParserArgumentDescription*> deprecatedArgs;

  // Loop over grouped argument descriptions
  QMapIterator<QString, QList<CommandLineParserArgumentDescription*> > it(
      this->Internal->GroupToArgumentDescriptionListMap);
  while(it.hasNext())
    {
    it.next();
    if (!it.key().isEmpty())
      {
      stream << "\n" << it.key() << "\n";
      }
    foreach(CommandLineParserArgumentDescription* argDesc, it.value())
      {
      if (argDesc->Deprecated)
        {
        deprecatedArgs << argDesc;
        }
      else
        {
        // Extract associated value from settings if any
        QString settingsValue;
        if (this->Internal->Settings)
          {
          QString key;
          if (!argDesc->LongArg.isEmpty())
            {
            key = argDesc->LongArg;
            }
          else
            {
            key = argDesc->ShortArg;
            }
          settingsValue = this->Internal->Settings->value(key).toString();
          }
        stream << argDesc->helpText(this->Internal->FieldWidth, charPad, settingsValue);
        }
      }
    }

  if (!deprecatedArgs.empty())
    {
    stream << "\nDeprecated arguments:\n";
    foreach(CommandLineParserArgumentDescription* argDesc, deprecatedArgs)
      {
      stream << argDesc->helpText(this->Internal->FieldWidth, charPad);
      }
    }

  return text;
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::argumentAdded(const QString& argument) const
{
  return this->Internal->ArgNameToArgumentDescriptionMap.contains(argument);
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::argumentParsed(const QString& argument) const
{
  return this->Internal->ProcessedArguments.contains(argument);
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::setArgumentPrefix(const QString& longPrefix, const QString& shortPrefix)
{
  this->Internal->LongPrefix = longPrefix;
  this->Internal->ShortPrefix = shortPrefix;
}

// --------------------------------------------------------------------------
void ctkCommandLineParser::setStrictModeEnabled(bool strictMode)
{
  this->Internal->StrictMode = strictMode;
}

