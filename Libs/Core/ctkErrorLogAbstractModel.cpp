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

// Qt includes
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMetaEnum>
#include <QMetaType>
#include <QMutexLocker>
#include <QPointer>
#include <QStringList>
#include <QThread>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkErrorLogAbstractModel.h"
#include "ctkErrorLogAbstractMessageHandler.h"
#include "ctkFileLogger.h"


// --------------------------------------------------------------------------
// ctkErrorLogAbstractModelPrivate

// --------------------------------------------------------------------------
class ctkErrorLogAbstractModelPrivate
{
  Q_DECLARE_PUBLIC(ctkErrorLogAbstractModel);
protected:
  ctkErrorLogAbstractModel* const q_ptr;
public:
  ctkErrorLogAbstractModelPrivate(ctkErrorLogAbstractModel& object);
  ~ctkErrorLogAbstractModelPrivate();

  void init(QAbstractItemModel* itemModel);

  void setMessageHandlerConnection(ctkErrorLogAbstractMessageHandler * msgHandler, bool asynchronous);

  QAbstractItemModel* ItemModel;

  QHash<QString, ctkErrorLogAbstractMessageHandler*> RegisteredHandlers;

  ctkErrorLogLevel::LogLevels CurrentLogLevelFilter;

  bool LogEntryGrouping;
  bool AsynchronousLogging;
  bool AddingEntry;

  ctkErrorLogLevel ErrorLogLevel;

  ctkErrorLogTerminalOutput StdErrTerminalOutput;
  ctkErrorLogTerminalOutput StdOutTerminalOutput;

  ctkFileLogger FileLogger;
  QString FileLoggingPattern;
};

// --------------------------------------------------------------------------
// ctkErrorLogAbstractModelPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogAbstractModelPrivate::ctkErrorLogAbstractModelPrivate(ctkErrorLogAbstractModel& object)
  : q_ptr(&object)
{
  qRegisterMetaType<ctkErrorLogContext>("ctkErrorLogContext");
  this->LogEntryGrouping = false;
  this->AsynchronousLogging = true;
  this->AddingEntry = false;
  this->FileLogger.setEnabled(false);
  this->FileLoggingPattern = "[%{level}][%{origin}] %{timestamp} [%{category}] (%{file}:%{line}) - %{msg}";
}

// --------------------------------------------------------------------------
ctkErrorLogAbstractModelPrivate::~ctkErrorLogAbstractModelPrivate()
{
  foreach(const QString& handlerName, this->RegisteredHandlers.keys())
    {
    ctkErrorLogAbstractMessageHandler * msgHandler =
        this->RegisteredHandlers.value(handlerName);
    Q_ASSERT(msgHandler);
    msgHandler->setEnabled(false);
    delete msgHandler;
    }
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModelPrivate::init(QAbstractItemModel* itemModel)
{
  Q_Q(ctkErrorLogAbstractModel);
  itemModel->setParent(q);
  q->setDynamicSortFilter(true);
  //
  // WARNING - Using a QSortFilterProxyModel slows down the insertion of rows by a factor 10
  //
  q->setSourceModel(itemModel);
  q->setFilterKeyColumn(ctkErrorLogAbstractModel::LogLevelColumn);

  this->ItemModel = itemModel;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModelPrivate::setMessageHandlerConnection(
    ctkErrorLogAbstractMessageHandler * msgHandler, bool asynchronous)
{
  Q_Q(ctkErrorLogAbstractModel);

  msgHandler->disconnect();

  QObject::connect(msgHandler,
        SIGNAL(messageHandled(QDateTime,QString,ctkErrorLogLevel::LogLevel,QString,ctkErrorLogContext,QString)),
        q, SLOT(addEntry(QDateTime,QString,ctkErrorLogLevel::LogLevel,QString,ctkErrorLogContext,QString)),
        asynchronous ? Qt::QueuedConnection : Qt::BlockingQueuedConnection);
}

// --------------------------------------------------------------------------
// ctkErrorLogAbstractModel methods

//------------------------------------------------------------------------------
ctkErrorLogAbstractModel::ctkErrorLogAbstractModel(QAbstractItemModel* itemModel, QObject * parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkErrorLogAbstractModelPrivate(*this))
{
  Q_D(ctkErrorLogAbstractModel);

  d->init(itemModel);
}

//------------------------------------------------------------------------------
ctkErrorLogAbstractModel::~ctkErrorLogAbstractModel()
{
}

//------------------------------------------------------------------------------
bool ctkErrorLogAbstractModel::registerMsgHandler(ctkErrorLogAbstractMessageHandler * msgHandler)
{
  Q_D(ctkErrorLogAbstractModel);
  if (!msgHandler)
    {
    return false;
    }
  if (d->RegisteredHandlers.keys().contains(msgHandler->handlerName()))
    {
    return false;
    }

  d->setMessageHandlerConnection(msgHandler, d->AsynchronousLogging);

  msgHandler->setTerminalOutput(ctkErrorLogTerminalOutput::StandardError, &d->StdErrTerminalOutput);
  msgHandler->setTerminalOutput(ctkErrorLogTerminalOutput::StandardOutput, &d->StdOutTerminalOutput);

  d->RegisteredHandlers.insert(msgHandler->handlerName(), msgHandler);
  return true;
}

//------------------------------------------------------------------------------
QStringList ctkErrorLogAbstractModel::msgHandlerNames()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->RegisteredHandlers.keys();
}

//------------------------------------------------------------------------------
bool ctkErrorLogAbstractModel::msgHandlerEnabled(const QString& handlerName) const
{
  Q_D(const ctkErrorLogAbstractModel);
  if (!d->RegisteredHandlers.keys().contains(handlerName))
    {
    return false;
    }
  return d->RegisteredHandlers.value(handlerName)->enabled();
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setMsgHandlerEnabled(const QString& handlerName, bool enabled)
{
  Q_D(ctkErrorLogAbstractModel);
  if (!d->RegisteredHandlers.keys().contains(handlerName))
    {
//    qCritical() << "Failed to enable/disable message handler " << handlerName
//                << "-  Handler not registered !";
    return;
    }
  d->RegisteredHandlers.value(handlerName)->setEnabled(enabled);
}

//------------------------------------------------------------------------------
QStringList ctkErrorLogAbstractModel::msgHandlerEnabled() const
{
  Q_D(const ctkErrorLogAbstractModel);
  QStringList msgHandlers;
  foreach(const QString& handlerName, d->RegisteredHandlers.keys())
    {
    if (d->RegisteredHandlers.value(handlerName)->enabled())
      {
      msgHandlers << handlerName;
      }
    }
  return msgHandlers;
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setMsgHandlerEnabled(const QStringList& handlerNames)
{
  foreach(const QString& handlerName, handlerNames)
    {
    this->setMsgHandlerEnabled(handlerName, true);
    }
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::enableAllMsgHandler()
{
  this->setAllMsgHandlerEnabled(true);
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::disableAllMsgHandler()
{
  this->setAllMsgHandlerEnabled(false);
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setAllMsgHandlerEnabled(bool enabled)
{
  Q_D(ctkErrorLogAbstractModel);
  foreach(const QString& msgHandlerName, d->RegisteredHandlers.keys())
    {
    this->setMsgHandlerEnabled(msgHandlerName, enabled);
    }
}

//------------------------------------------------------------------------------
ctkErrorLogTerminalOutput::TerminalOutputs ctkErrorLogAbstractModel::terminalOutputs()const
{
  Q_D(const ctkErrorLogAbstractModel);
  ctkErrorLogTerminalOutput::TerminalOutputs currentTerminalOutputs;
  currentTerminalOutputs |= d->StdErrTerminalOutput.enabled() ? ctkErrorLogTerminalOutput::StandardError : ctkErrorLogTerminalOutput::None;
  currentTerminalOutputs |= d->StdOutTerminalOutput.enabled() ? ctkErrorLogTerminalOutput::StandardOutput : ctkErrorLogTerminalOutput::None;
  return currentTerminalOutputs;
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setTerminalOutputs(
    const ctkErrorLogTerminalOutput::TerminalOutputs& terminalOutput)
{
  Q_D(ctkErrorLogAbstractModel);
  d->StdErrTerminalOutput.setEnabled(terminalOutput & ctkErrorLogTerminalOutput::StandardOutput);
  d->StdOutTerminalOutput.setEnabled(terminalOutput & ctkErrorLogTerminalOutput::StandardError);
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::addEntry(const QDateTime& currentDateTime, const QString& threadId,
                                ctkErrorLogLevel::LogLevel logLevel,
                                const QString& origin, const ctkErrorLogContext &context, const QString &text)
{
  Q_D(ctkErrorLogAbstractModel);

  if (d->AddingEntry)
    {
    return;
    }

  d->AddingEntry = true;

  QString timeFormat("dd.MM.yyyy hh:mm:ss");

  bool groupEntry = false;
  if (d->LogEntryGrouping)
    {
    int lastRowIndex = d->ItemModel->rowCount() - 1;

    QString lastRowThreadId = this->logEntryData(lastRowIndex, Self::ThreadIdColumn).toString();
    bool threadIdMatched = threadId == lastRowThreadId;

    QString lastRowLogLevel = this->logEntryData(lastRowIndex, Self::LogLevelColumn).toString();
    bool logLevelMatched = d->ErrorLogLevel(logLevel) == lastRowLogLevel;

    QString lastRowOrigin = this->logEntryData(lastRowIndex, Self::OriginColumn).toString();
    bool originMatched = origin == lastRowOrigin;

    QDateTime lastRowDateTime =
        QDateTime::fromString(this->logEntryData(lastRowIndex, Self::TimeColumn).toString(), timeFormat);
    int groupingIntervalInMsecs = 1000;
    bool withinGroupingInterval = lastRowDateTime.time().msecsTo(currentDateTime.time()) <= groupingIntervalInMsecs;

    groupEntry = threadIdMatched && logLevelMatched && originMatched && withinGroupingInterval;
    }

  if (!groupEntry)
    {
    this->addModelEntry(
      currentDateTime.toString(timeFormat), threadId, d->ErrorLogLevel(logLevel), origin, text);
    }
  else
    {
    // Retrieve description associated with last row
    QModelIndex lastRowDescriptionIndex =
        d->ItemModel->index(d->ItemModel->rowCount() - 1, ctkErrorLogAbstractModel::DescriptionColumn);

    QStringList updatedDescription;
    updatedDescription << lastRowDescriptionIndex.data(ctkErrorLogAbstractModel::DescriptionTextRole).toString();
    updatedDescription << text;

    d->ItemModel->setData(lastRowDescriptionIndex, updatedDescription.join("\n"),
                                 ctkErrorLogAbstractModel::DescriptionTextRole);

    // Append '...' to displayText if needed
    QString displayText = lastRowDescriptionIndex.data().toString();
    if (!displayText.endsWith("..."))
      {
      d->ItemModel->setData(lastRowDescriptionIndex, displayText.append("..."), Qt::DisplayRole);
      }
    }

  d->AddingEntry = false;

  QString fileLogText = d->FileLoggingPattern;
  fileLogText.replace("%{level}", d->ErrorLogLevel(logLevel).toUpper());
  fileLogText.replace("%{timestamp}", currentDateTime.toString(timeFormat));
  fileLogText.replace("%{origin}", origin);
  fileLogText.replace("%{pid}", QString("%1").arg(QCoreApplication::applicationPid()));
  fileLogText.replace("%{threadid}", threadId);
  fileLogText.replace("%{function}", context.Function);
  fileLogText.replace("%{line}", QString("%1").arg(context.Line));
  fileLogText.replace("%{file}", context.File);
  fileLogText.replace("%{category}", context.Category);
  fileLogText.replace("%{msg}", context.Message);
  d->FileLogger.logMessage(fileLogText.trimmed());

  emit this->entryAdded(logLevel);
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::clear()
{
  Q_D(ctkErrorLogAbstractModel);
  d->ItemModel->removeRows(0, d->ItemModel->rowCount());
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::filterEntry(const ctkErrorLogLevel::LogLevels& logLevel,
                                   bool disableFilter)
{
  Q_D(ctkErrorLogAbstractModel);

  QStringList patterns;
  if (!this->filterRegExp().pattern().isEmpty())
    {
    patterns << this->filterRegExp().pattern().split("|");
    }
  patterns.removeAll(d->ErrorLogLevel(ctkErrorLogLevel::None));

//  foreach(QString s, patterns)
//    {
//    std::cout << "pattern:" << qPrintable(s) << std::endl;
//    }

  QMetaEnum logLevelEnum = d->ErrorLogLevel.metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);

  // Loop over enum values and append associated name to 'patterns' if
  // it has been specified within 'logLevel'
  for (int i = 1; i < logLevelEnum.keyCount(); ++i)
    {
    int aLogLevel = logLevelEnum.value(i);
    if (logLevel & aLogLevel)
      {
      QString logLevelAsString = d->ErrorLogLevel(static_cast<ctkErrorLogLevel::LogLevel>(aLogLevel));
      if (!disableFilter)
        {
        patterns << logLevelAsString;
        d->CurrentLogLevelFilter |= static_cast<ctkErrorLogLevel::LogLevels>(aLogLevel);
        }
      else
        {
        patterns.removeAll(logLevelAsString);
        d->CurrentLogLevelFilter ^= static_cast<ctkErrorLogLevel::LogLevels>(aLogLevel);
        }
      }
    }

  if (patterns.isEmpty())
    {
    // If there are no patterns, let's filter with the None level so that
    // all entries are filtered out.
    patterns << d->ErrorLogLevel(ctkErrorLogLevel::None);
    }

  bool filterChanged = true;
  QStringList currentPatterns = this->filterRegExp().pattern().split("|");
  if (currentPatterns.count() == patterns.count())
    {
    foreach(const QString& p, patterns)
      {
      currentPatterns.removeAll(p);
      }
    filterChanged = currentPatterns.count() > 0;
    }

  this->setFilterRegExp(patterns.join("|"));

  if (filterChanged)
    {
    emit this->logLevelFilterChanged();
    }
}

//------------------------------------------------------------------------------
ctkErrorLogLevel::LogLevels ctkErrorLogAbstractModel::logLevelFilter()const
{
  Q_D(const ctkErrorLogAbstractModel);
  QMetaEnum logLevelEnum = d->ErrorLogLevel.metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);

  ctkErrorLogLevel::LogLevels filter = ctkErrorLogLevel::Unknown;
  foreach(const QString& filterAsString, this->filterRegExp().pattern().split("|"))
    {
    filter |= static_cast<ctkErrorLogLevel::LogLevels>(logLevelEnum.keyToValue(filterAsString.toLatin1()));
    }
  return filter;
}

//------------------------------------------------------------------------------
bool ctkErrorLogAbstractModel::logEntryGrouping()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->LogEntryGrouping;
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setLogEntryGrouping(bool value)
{
  Q_D(ctkErrorLogAbstractModel);
  d->LogEntryGrouping = value;
}

//------------------------------------------------------------------------------
bool ctkErrorLogAbstractModel::asynchronousLogging()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->AsynchronousLogging;
}

//------------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setAsynchronousLogging(bool value)
{
  Q_D(ctkErrorLogAbstractModel);
  if (d->AsynchronousLogging == value)
    {
    return;
    }

  foreach(const QString& handlerName, d->RegisteredHandlers.keys())
    {
    d->setMessageHandlerConnection(
          d->RegisteredHandlers.value(handlerName), value);
    }

  d->AsynchronousLogging = value;
}

// --------------------------------------------------------------------------
QString ctkErrorLogAbstractModel::filePath()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->FileLogger.filePath();
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setFilePath(const QString& filePath)
{
  Q_D(ctkErrorLogAbstractModel);
  return d->FileLogger.setFilePath(filePath);
}

// --------------------------------------------------------------------------
int ctkErrorLogAbstractModel::numberOfFilesToKeep()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->FileLogger.numberOfFilesToKeep();
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setNumberOfFilesToKeep(int value)
{
  Q_D(ctkErrorLogAbstractModel);
  return d->FileLogger.setNumberOfFilesToKeep(value);
}

// --------------------------------------------------------------------------
bool ctkErrorLogAbstractModel::fileLoggingEnabled()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->FileLogger.enabled();
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setFileLoggingEnabled(bool value)
{
  Q_D(ctkErrorLogAbstractModel);
  d->FileLogger.setEnabled(value);
}

// --------------------------------------------------------------------------
QString ctkErrorLogAbstractModel::fileLoggingPattern()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->FileLoggingPattern;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractModel::setFileLoggingPattern(const QString& value)
{
  Q_D(ctkErrorLogAbstractModel);
  d->FileLoggingPattern = value;
}

// --------------------------------------------------------------------------
QVariant ctkErrorLogAbstractModel::logEntryData(int row, int column, int role) const
{
  Q_D(const ctkErrorLogAbstractModel);
  if (column < 0 || column > Self::MaxColumn
      || row < 0 || row > this->logEntryCount())
    {
    return QVariant();
    }
  QModelIndex rowDescriptionIndex = d->ItemModel->index(row, column);
  return rowDescriptionIndex.data(role);
}

// --------------------------------------------------------------------------
QString ctkErrorLogAbstractModel::logEntryDescription(int row) const
{
  return this->logEntryData(row, Self::DescriptionColumn, Self::DescriptionTextRole).toString();
}

// --------------------------------------------------------------------------
int ctkErrorLogAbstractModel::logEntryCount()const
{
  Q_D(const ctkErrorLogAbstractModel);
  return d->ItemModel->rowCount();
}
