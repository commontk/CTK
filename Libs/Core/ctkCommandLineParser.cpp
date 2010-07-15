
// Qt includes 
#include <QHash>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

// CTK includes
#include "ctkCommandLineParser.h"

namespace
{

class CommandLineParserArgumentDescription
{
public:
  CommandLineParserArgumentDescription(
    const QString& longArg, const QString& shortArg, QVariant::Type type,
    const QString& argHelp, const QVariant& defaultValue, bool ignoreRest)
      : LongArg(longArg), ShortArg(shortArg), ArgHelp(argHelp),
  IgnoreRest(ignoreRest), NumberOfParametersToProcess(0),
  Value(type)
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
    default:
      ExactMatchFailedMessage = QString("Type %1 not supported.").arg(static_cast<int>(type));
    }

  }

  ~CommandLineParserArgumentDescription(){}

  bool addParameter(const QString& value);

  QString helpText(int fieldWidth, const char charPad);

  QString LongArg;
  QString ShortArg;
  QString ArgHelp;
  bool    IgnoreRest;
  int     NumberOfParametersToProcess;
  QString RegularExpression;
  QString ExactMatchFailedMessage;

  QVariant Value;
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
  default:
    return false;
  }

  return true;
}

// --------------------------------------------------------------------------
QString CommandLineParserArgumentDescription::helpText(int fieldWidth, const char charPad)
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

}

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal class

// --------------------------------------------------------------------------
class ctkCommandLineParser::ctkInternal
{
public:
  ctkInternal():Debug(false),FieldWidth(0){}

  ~ctkInternal() { qDeleteAll(ArgumentDescriptionList); }
  
  CommandLineParserArgumentDescription* argumentDescription(const QString& argument);
  
  QList<CommandLineParserArgumentDescription*>          ArgumentDescriptionList;
  QHash<QString, CommandLineParserArgumentDescription*> ArgNameToArgumentDescriptionMap;
  
  QStringList UnparsedArguments; 
  QStringList ProcessedArguments;
  QString     ErrorString;
  bool        Debug;
  int         FieldWidth;
};

// --------------------------------------------------------------------------
// ctkCommandLineParser::ctkInternal methods

// --------------------------------------------------------------------------
CommandLineParserArgumentDescription*
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
QHash<QString, QVariant> ctkCommandLineParser::parseArguments(const QStringList& arguments,
                                                              bool* ok)
{
  // Reset
  this->Internal->UnparsedArguments.clear();
  this->Internal->ProcessedArguments.clear();
  this->Internal->ErrorString.clear();

  bool ignoreRest = false;
  CommandLineParserArgumentDescription * currentArgDesc = 0;
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
            if (ok) *ok = false;
            return QHash<QString, QVariant>();
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
            if (ok) *ok = false;
            return QHash<QString, QVariant>();
            }
          if (!currentArgDesc->addParameter(parameter))
            {
            this->Internal->ErrorString = QString(
                "Value(s) associated with argument %1 are incorrect. %2").
                arg(argument).arg(currentArgDesc->ExactMatchFailedMessage);

            if (this->Internal->Debug) { qDebug() << this->Internal->ErrorString; }
            if (ok) *ok = false;
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
            if (ok) *ok = false;
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
      this->Internal->UnparsedArguments << argument;
      }
    }

  if (ok) *ok = true;

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

    parsedArguments.insert(key, desc->Value);
  }
  return parsedArguments;
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

// --------------------------------------------------------------------------
void ctkCommandLineParser::addArgument(const QString& longarg, const QString& shortarg,
                                       QVariant::Type type, const QString& argHelp,
                                       const QVariant& defaultValue, bool ignoreRest)
{
  Q_ASSERT(!defaultValue.isValid() || defaultValue.type() == type);

  /* Make sure it's not already added */
  bool added = this->Internal->ArgNameToArgumentDescriptionMap.contains(longarg);
  Q_ASSERT(!added);
  if (added) { return; }

  added = this->Internal->ArgNameToArgumentDescriptionMap.contains(shortarg);
  Q_ASSERT(!added);
  if (added) { return; }

  CommandLineParserArgumentDescription* argDesc =
    new CommandLineParserArgumentDescription(longarg, shortarg, type,
                                             argHelp, defaultValue, ignoreRest);

  Q_ASSERT(!(longarg.isEmpty() && shortarg.isEmpty()));
  if (longarg.isEmpty() && shortarg.isEmpty()) { return; }

  if (!longarg.isEmpty())
  {
    this->Internal->ArgNameToArgumentDescriptionMap[longarg] = argDesc;
    int argWidth = longarg.length() + 7;
    if (argWidth > this->Internal->FieldWidth)
    {
      this->Internal->FieldWidth = argWidth;
    }
  }
  if (!shortarg.isEmpty())
  {
    this->Internal->ArgNameToArgumentDescriptionMap[shortarg] = argDesc;
    int argWidth = shortarg.length() + 7;
    if (argWidth > this->Internal->FieldWidth)
    {
      this->Internal->FieldWidth = argWidth;
    }
  }

  this->Internal->ArgumentDescriptionList << argDesc;
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
QString ctkCommandLineParser::helpText(const char charPad)
{
  QString text;
  QTextStream stream(&text);

  // Loop over argument descriptions
  foreach(CommandLineParserArgumentDescription* argDesc,
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

