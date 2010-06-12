
// Qt includes 
#include <QHash>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

// CTK includes
#include "ctkCommandLineParser.h"

namespace
{

class CommandLineParserArgumentDescriptionBase
{
public:
  CommandLineParserArgumentDescriptionBase(
    const char* longArg, const char* shortArg, const QString& argHelp, bool ignoreRest)
    {
    this->LongArg = QLatin1String(longArg);
    this->ShortArg = QLatin1String(shortArg);
    this->ArgHelp = argHelp;
    this->IgnoreRest = ignoreRest;
    this->NumberOfParametersToProcess = 0;
    }
  virtual bool addParameter(const QString& value) = 0;
  QString helpText(int fieldWidth, const char charPad);
  QString LongArg;
  QString ShortArg;
  QString ArgHelp;
  bool    IgnoreRest;
  int     NumberOfParametersToProcess;
  QString RegularExpression;
  QString ExactMatchFailedMessage;
  QString ArgumentType;
};

// --------------------------------------------------------------------------
QString CommandLineParserArgumentDescriptionBase::helpText(int fieldWidth, const char charPad)
{
  QString text;
  QTextStream stream(&text);
  stream.setFieldAlignment(QTextStream::AlignLeft);
  stream.setPadChar(charPad);

  if (this->LongArg.isEmpty() && !this->ArgHelp.isEmpty())
    {
    stream.setFieldWidth(fieldWidth);
    }

  if (!this->ShortArg.isEmpty())
    {
    stream  << QString("  %1").arg(this->ShortArg);
    if(!this->LongArg.isEmpty())
      {
      stream << "\n";
      }
    }

  if (!this->LongArg.isEmpty())
    {
    if(!this->ArgHelp.isEmpty())
      {
      stream.setFieldWidth(fieldWidth);
      }
    stream  << QString("  %1").arg(this->LongArg);
    }
  stream.setFieldWidth(0);
  stream << this->ArgHelp << "\n";
  return text;
}

#define CommandLineParserArgumentDescription_class(_NAME, _TYPE,                        \
                                                   _NUMBEROFPARAMTOPROCESS,             \
                                                   _REGEXP, _EXACTMACTHERRORMSG)        \
  class CommandLineParser##_NAME##ArgumentDescription:                                  \
    public CommandLineParserArgumentDescriptionBase                                     \
  {                                                                                     \
  public:                                                                               \
    CommandLineParser##_NAME##ArgumentDescription(                                      \
      const char* longArg, const char* shortArg, _TYPE * variable,                      \
      const QString& argHelp, const _TYPE& defaultValue,                                \
      bool ignoreRest):                                                                 \
      CommandLineParserArgumentDescriptionBase(longArg , shortArg, argHelp, ignoreRest) \
      {                                                                                 \
      this->Variable = variable;                                                        \
      this->DefaultValue = defaultValue;                                                \
      this->NumberOfParametersToProcess = _NUMBEROFPARAMTOPROCESS;                      \
      this->RegularExpression = _REGEXP;                                                \
      this->ArgumentType = #_TYPE;                                                      \
      }                                                                                 \
     virtual bool addParameter(const QString& value);                                   \
    _TYPE* Variable;                                                                    \
    _TYPE DefaultValue;                                                                 \
  };

CommandLineParserArgumentDescription_class(String, QString, 1, ".*", "");
CommandLineParserArgumentDescription_class(Boolean, bool, 0, "", "");
CommandLineParserArgumentDescription_class(StringList, QStringList, -1, ".*", "");
CommandLineParserArgumentDescription_class(Integer, int, 1, "-?[0-9]+",
                                           "A negative or positive integer is expected.");

#undef CommandLineParserArgumentDescription_class

// --------------------------------------------------------------------------
bool CommandLineParserStringArgumentDescription::addParameter(const QString& value)
{
  // Validate value
  QRegExp regexp(this->RegularExpression);
  if (!regexp.exactMatch(value))
    {
    return false;
    }
  (*this->Variable).clear();
  (*this->Variable).append(value);
  return true;
}

// --------------------------------------------------------------------------
bool CommandLineParserBooleanArgumentDescription::addParameter(const QString& value)
{
  *this->Variable = (value == "true");
  return true;
}

// --------------------------------------------------------------------------
bool CommandLineParserStringListArgumentDescription::addParameter(const QString& value)
{
  // Validate value
  QRegExp regexp(this->RegularExpression);
  if (!regexp.exactMatch(value))
    {
    return false;
    }
  *this->Variable << value;
  return true;
}

// --------------------------------------------------------------------------
bool CommandLineParserIntegerArgumentDescription::addParameter(const QString& value)
{
  // Validate value
  QRegExp regexp(this->RegularExpression);
  if (!regexp.exactMatch(value))
    {
    return false;
    }
  *this->Variable = value.toInt();
  return true;
}

}

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal class

// --------------------------------------------------------------------------
class ctkCommandLineParser::ctkInternal
{
public:
  ctkInternal():Debug(false),FieldWidth(0){}
  
  CommandLineParserArgumentDescriptionBase* argumentDescription(const QString& argument);
  
  QList<CommandLineParserArgumentDescriptionBase*>          ArgumentDescriptionList;
  QHash<QString, CommandLineParserArgumentDescriptionBase*> ArgNameToArgumentDescriptionMap;
  
  #define ctkCommandLineParser_ctkInternal_declare_map(_NAME) \
    QHash<QString, CommandLineParser##_NAME##ArgumentDescription*>       \
      LongArgTo##_NAME##ArgumentDescriptionMap;                          \
    QHash<QString, CommandLineParser##_NAME##ArgumentDescription*>       \
      ShortArgTo##_NAME##ArgumentDescriptionMap;
  
  ctkCommandLineParser_ctkInternal_declare_map(String);
  ctkCommandLineParser_ctkInternal_declare_map(Boolean);
  ctkCommandLineParser_ctkInternal_declare_map(StringList);
  ctkCommandLineParser_ctkInternal_declare_map(Integer);
  
  #undef ctkCommandLineParser_ctkInternal_declare_map
  
  QStringList UnparsedArguments; 
  QStringList ProcessedArguments;
  QString     ErrorString;
  bool        Debug;
  int         FieldWidth;
};

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal methods

// --------------------------------------------------------------------------
CommandLineParserArgumentDescriptionBase* 
  ctkCommandLineParser::ctkInternal::argumentDescription(const QString& argument)
{
  if (this->ArgNameToArgumentDescriptionMap.contains(argument))
    {
    return this->ArgNameToArgumentDescriptionMap[argument];
    }
  return 0;
}

// --------------------------------------------------------------------------
// ctkCommandLineParser methods

// --------------------------------------------------------------------------
ctkCommandLineParser::ctkCommandLineParser()
{
  this->Internal = new ctkInternal();
}

// --------------------------------------------------------------------------
ctkCommandLineParser::~ctkCommandLineParser()
{
  delete this->Internal;
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::parseArguments(const QStringList& arguments)
{
  // Reset
  this->Internal->UnparsedArguments.clear();
  this->Internal->ProcessedArguments.clear();
  this->Internal->ErrorString.clear();

  bool ignoreRest = false;
  CommandLineParserArgumentDescriptionBase * currentArgDesc = 0;
  for(int i = 1; i < arguments.size(); ++i)
    {

    QString argument = arguments.at(i);
    if (this->Internal->Debug) { qDebug() << "Processing" << argument; }

    // should argument be ignored ?
    if (ignoreRest)
      {
      this->Internal->UnparsedArguments << argument;
      continue;
      }

    // Skip if argument has already been parsed ...
    if (this->Internal->ProcessedArguments.contains(argument))
      {
      qDebug() << "Skipping argument" << argument << " - Already processed !";
      continue;
      }

    // Retrieve corresponding argument description
    currentArgDesc = this->Internal->argumentDescription(argument);

    // Is there a corresponding argument description ?
    if (currentArgDesc)
      {
      this->Internal->ProcessedArguments << currentArgDesc->ShortArg << currentArgDesc->LongArg;
      int numberOfParametersToProcess = currentArgDesc->NumberOfParametersToProcess;
      ignoreRest = currentArgDesc->IgnoreRest;

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
            return false;
            }
          QString parameter = arguments.at(i + j);
          if (this->Internal->Debug)
            {
            qDebug() << "Processing parameter" << j << ", value:" << parameter;
            }
          if (this->argumentAdded(parameter))
            {
            this->Internal->ErrorString =
                missingParameterError.arg(argument).arg(j-1).arg(numberOfParametersToProcess);
            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            return false;
            }
          if (!currentArgDesc->addParameter(parameter))
            {
            this->Internal->ErrorString = QString(
                "Value(s) associated with argument %1 are incorrect. %2").
                arg(argument).arg(currentArgDesc->ExactMatchFailedMessage);

            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            return false;
            }
          }
        // Update main loop increment
        i = i + numberOfParametersToProcess;
        }
      else if (numberOfParametersToProcess == -1)
        {
        if (this->Internal->Debug)
          {
          qDebug() << "Proccessing StringList ...";
          }
        int j = 1;
        while(j + i < arguments.size())
          {
          if (this->argumentAdded(arguments.at(j + i)))
            {
            if (this->Internal->Debug)
              {
              qDebug() << "No more parameter for" << argument;
              }
            break;
            }
          QString parameter = arguments.at(j + i);
          if (this->Internal->Debug)
            {
            qDebug() << "Processing parameter" << j << ", value:" << parameter;
            }
          if (!currentArgDesc->addParameter(parameter))
            {
            this->Internal->ErrorString = QString(
                "Value(s) associated with argument %1 are incorrect. %2").
                arg(argument).arg(currentArgDesc->ExactMatchFailedMessage);

            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            return false;
            }
          j++;
          }
        // Update main loop increment
        i = i + j;
        }
      }
    else
      {
      this->Internal->UnparsedArguments << argument;
      }
    }
  return true;
}

// -------------------------------------------------------------------------
QString ctkCommandLineParser::errorString()
{
  return this->Internal->ErrorString;
}

// -------------------------------------------------------------------------
const QStringList& ctkCommandLineParser::unparsedArguments()
{
  return this->Internal->UnparsedArguments;
}

// -------------------------------------------------------------------------
#define ctkCommandLineParser_addArgument_cxx_core(_NAME, _TYPE)                 \
  /* Make sure it's not already added */                                                   \
  bool added = this->Internal->LongArgTo##_NAME##ArgumentDescriptionMap.contains(longarg); \
  Q_ASSERT(!added);                                                                        \
  if (added) { return; }                                                                   \
                                                                                           \
  added = this->Internal->ShortArgTo##_NAME##ArgumentDescriptionMap.contains(shortarg);    \
  Q_ASSERT(!added);                                                                        \
  if (added) { return; }                                                                   \
                                                                                           \
  CommandLineParser##_NAME##ArgumentDescription * argDesc =                                \
    new CommandLineParser##_NAME##ArgumentDescription(longarg, shortarg, var,              \
                                                      argHelp, defaultValue, ignoreRest);  \
                                                                                           \
  Q_ASSERT(!(longarg == 0 && shortarg == 0));                                              \
  if (longarg == 0 && shortarg == 0) { return; }                                           \
  if (longarg != 0)                                                                        \
    {                                                                                      \
    this->Internal->LongArgTo##_NAME##ArgumentDescriptionMap[longarg] = argDesc;           \
    int argWidth = QString(longarg).length() + 7;                                          \
    if (argWidth > this->Internal->FieldWidth)                                             \
      {                                                                                    \
      this->Internal->FieldWidth = argWidth;                                               \
      }                                                                                    \
    }                                                                                      \
  if (shortarg != 0)                                                                       \
    {                                                                                      \
    this->Internal->ShortArgTo##_NAME##ArgumentDescriptionMap[shortarg] = argDesc;         \
    int argWidth = QString(shortarg).length() + 7;                                         \
    if (argWidth > this->Internal->FieldWidth)                                             \
      {                                                                                    \
      this->Internal->FieldWidth = argWidth;                                               \
      }                                                                                    \
    }                                                                                      \
  this->Internal->ArgNameToArgumentDescriptionMap[longarg] = argDesc;                      \
  this->Internal->ArgNameToArgumentDescriptionMap[shortarg] = argDesc;                     \
  this->Internal->ArgumentDescriptionList << argDesc;

  // -------------------------------------------------------------------------
  #define ctkCommandLineParser_addArgument_cxx(_NAME, _TYPE)                        \
    void ctkCommandLineParser::add##_NAME##Argument(const char* longarg,            \
      const char* shortarg, _TYPE* var, const QString& argHelp, const _TYPE& defaultValue,     \
      bool ignoreRest)                                                                         \
    {                                                                                          \
    ctkCommandLineParser_addArgument_cxx_core(_NAME, _TYPE);                        \
    }

  // -------------------------------------------------------------------------
  #define ctkCommandLineParser_addArgument_cxx_without_ignore_rest(_NAME, _TYPE)    \
    void ctkCommandLineParser::add##_NAME##Argument(const char* longarg,            \
      const char* shortarg, _TYPE* var, const QString& argHelp, const _TYPE& defaultValue)     \
    {                                                                                          \
    bool ignoreRest = false;                                                                   \
    ctkCommandLineParser_addArgument_cxx_core(_NAME, _TYPE);                        \
    }

// --------------------------------------------------------------------------
ctkCommandLineParser_addArgument_cxx(String, QString);
ctkCommandLineParser_addArgument_cxx(Boolean, bool);
ctkCommandLineParser_addArgument_cxx_without_ignore_rest(StringList, QStringList);
ctkCommandLineParser_addArgument_cxx(Integer, int);

#undef ctkCommandLineParser_addArgument_cxx

// --------------------------------------------------------------------------
bool ctkCommandLineParser::setExactMatchRegularExpression(
    const QString& argument, const QString& expression, const QString& exactMatchFailedMessage)
{
  CommandLineParserArgumentDescriptionBase * argDesc =
      this->Internal->argumentDescription(argument);
  if (!argDesc)
    {
    return false;
    }

  if (argDesc->ArgumentType == "bool")
    {
    return false;
    }
  argDesc->RegularExpression = expression;
  argDesc->ExactMatchFailedMessage = exactMatchFailedMessage;
  return true;
}

// --------------------------------------------------------------------------
QString ctkCommandLineParser::helpText(const char charPad)
{
  QString text;
  QTextStream stream(&text);

  // Loop over argument descriptions
  foreach(CommandLineParserArgumentDescriptionBase* argDesc,
          this->Internal->ArgumentDescriptionList)
    {
    stream << argDesc->helpText(this->Internal->FieldWidth, charPad);
    }
  return text;
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::argumentAdded(const QString& argument)
{
  return this->Internal->ArgNameToArgumentDescriptionMap.contains(argument);
}

// --------------------------------------------------------------------------
bool ctkCommandLineParser::argumentParsed(const QString& argument)
{
  return this->Internal->ProcessedArguments.contains(argument);
}

