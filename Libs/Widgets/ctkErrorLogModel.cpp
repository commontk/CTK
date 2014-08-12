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
#include <QStandardItem>
#include <QThread>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkErrorLogModel.h"
#include "ctkErrorLogAbstractMessageHandler.h"
#include "ctkFileLogger.h"


// --------------------------------------------------------------------------
// ctkErrorLogModelPrivate

// --------------------------------------------------------------------------
class ctkErrorLogModelPrivate
{
  Q_DECLARE_PUBLIC(ctkErrorLogModel);
protected:
  ctkErrorLogModel* const q_ptr;
public:
  ctkErrorLogModelPrivate(ctkErrorLogModel& object);
  ~ctkErrorLogModelPrivate();

  void init();

  void setMessageHandlerConnection(ctkErrorLogAbstractMessageHandler * msgHandler, bool asynchronous);

  QStandardItemModel StandardItemModel;

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
// ctkErrorLogModelPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::ctkErrorLogModelPrivate(ctkErrorLogModel& object)
  : q_ptr(&object)
{
  qRegisterMetaType<ctkErrorLogContext>("ctkErrorLogContext");
  this->StandardItemModel.setColumnCount(ctkErrorLogModel::MaxColumn);
  this->LogEntryGrouping = false;
  this->AsynchronousLogging = true;
  this->AddingEntry = false;
  this->FileLogger.setEnabled(false);
  this->FileLoggingPattern = "[%{level}][%{origin}] %{timestamp} [%{category}] (%{file}:%{line}) - %{msg}";
}

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::~ctkErrorLogModelPrivate()
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
void ctkErrorLogModelPrivate::init()
{
  Q_Q(ctkErrorLogModel);
  q->setDynamicSortFilter(true);
  //
  // WARNING - Using a QSortFilterProxyModel slows down the insertion of rows by a factor 10
  //
  q->setSourceModel(&this->StandardItemModel);
  q->setFilterKeyColumn(ctkErrorLogModel::LogLevelColumn);
}

// --------------------------------------------------------------------------
void ctkErrorLogModelPrivate::setMessageHandlerConnection(
    ctkErrorLogAbstractMessageHandler * msgHandler, bool asynchronous)
{
  Q_Q(ctkErrorLogModel);

  msgHandler->disconnect();

  QObject::connect(msgHandler,
        SIGNAL(messageHandled(QDateTime,QString,ctkErrorLogLevel::LogLevel,QString,ctkErrorLogContext,QString)),
        q, SLOT(addEntry(QDateTime,QString,ctkErrorLogLevel::LogLevel,QString,ctkErrorLogContext,QString)),
        asynchronous ? Qt::QueuedConnection : Qt::BlockingQueuedConnection);
}

// --------------------------------------------------------------------------
// ctkErrorLogModel methods

//------------------------------------------------------------------------------
ctkErrorLogModel::ctkErrorLogModel(QObject * parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkErrorLogModelPrivate(*this))
{
  Q_D(ctkErrorLogModel);

  d->init();
}

//------------------------------------------------------------------------------
ctkErrorLogModel::~ctkErrorLogModel()
{
}

//------------------------------------------------------------------------------
bool ctkErrorLogModel::registerMsgHandler(ctkErrorLogAbstractMessageHandler * msgHandler)
{
  Q_D(ctkErrorLogModel);
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
QStringList ctkErrorLogModel::msgHandlerNames()const
{
  Q_D(const ctkErrorLogModel);
  return d->RegisteredHandlers.keys();
}

//------------------------------------------------------------------------------
bool ctkErrorLogModel::msgHandlerEnabled(const QString& handlerName) const
{
  Q_D(const ctkErrorLogModel);
  if (!d->RegisteredHandlers.keys().contains(handlerName))
    {
    return false;
    }
  return d->RegisteredHandlers.value(handlerName)->enabled();
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setMsgHandlerEnabled(const QString& handlerName, bool enabled)
{
  Q_D(ctkErrorLogModel);
  if (!d->RegisteredHandlers.keys().contains(handlerName))
    {
//    qCritical() << "Failed to enable/disable message handler " << handlerName
//                << "-  Handler not registered !";
    return;
    }
  d->RegisteredHandlers.value(handlerName)->setEnabled(enabled);
}

//------------------------------------------------------------------------------
QStringList ctkErrorLogModel::msgHandlerEnabled() const
{
  Q_D(const ctkErrorLogModel);
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
void ctkErrorLogModel::setMsgHandlerEnabled(const QStringList& handlerNames)
{
  foreach(const QString& handlerName, handlerNames)
    {
    this->setMsgHandlerEnabled(handlerName, true);
    }
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::enableAllMsgHandler()
{
  this->setAllMsgHandlerEnabled(true);
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::disableAllMsgHandler()
{
  this->setAllMsgHandlerEnabled(false);
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setAllMsgHandlerEnabled(bool enabled)
{
  Q_D(ctkErrorLogModel);
  foreach(const QString& msgHandlerName, d->RegisteredHandlers.keys())
    {
    this->setMsgHandlerEnabled(msgHandlerName, enabled);
    }
}

//------------------------------------------------------------------------------
ctkErrorLogTerminalOutput::TerminalOutputs ctkErrorLogModel::terminalOutputs()const
{
  Q_D(const ctkErrorLogModel);
  ctkErrorLogTerminalOutput::TerminalOutputs currentTerminalOutputs;
  currentTerminalOutputs |= d->StdErrTerminalOutput.enabled() ? ctkErrorLogTerminalOutput::StandardError : ctkErrorLogTerminalOutput::None;
  currentTerminalOutputs |= d->StdOutTerminalOutput.enabled() ? ctkErrorLogTerminalOutput::StandardOutput : ctkErrorLogTerminalOutput::None;
  return currentTerminalOutputs;
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setTerminalOutputs(
    const ctkErrorLogTerminalOutput::TerminalOutputs& terminalOutput)
{
  Q_D(ctkErrorLogModel);
  d->StdErrTerminalOutput.setEnabled(terminalOutput & ctkErrorLogTerminalOutput::StandardOutput);
  d->StdOutTerminalOutput.setEnabled(terminalOutput & ctkErrorLogTerminalOutput::StandardError);
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::addEntry(const QDateTime& currentDateTime, const QString& threadId,
                                ctkErrorLogLevel::LogLevel logLevel,
                                const QString& origin, const ctkErrorLogContext &context, const QString &text)
{
  Q_D(ctkErrorLogModel);

  if (d->AddingEntry)
    {
    return;
    }

  d->AddingEntry = true;

  QString timeFormat("dd.MM.yyyy hh:mm:ss");

  bool groupEntry = false;
  if (d->LogEntryGrouping)
    {
    int lastRowIndex = d->StandardItemModel.rowCount() - 1;

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
    QList<QStandardItem*> itemList;

    // Time item
    QStandardItem * timeItem = new QStandardItem(currentDateTime.toString(timeFormat));
    timeItem->setEditable(false);
    itemList << timeItem;

    // ThreadId item
    QStandardItem * threadIdItem = new QStandardItem(threadId);
    threadIdItem->setEditable(false);
    itemList << threadIdItem;

    // LogLevel item
    QStandardItem * logLevelItem = new QStandardItem(d->ErrorLogLevel(logLevel));
    logLevelItem->setEditable(false);
    itemList << logLevelItem;

    // Origin item
    QStandardItem * originItem = new QStandardItem(origin);
    originItem->setEditable(false);
    itemList << originItem;

    // Description item
    QStandardItem * descriptionItem = new QStandardItem();
    QString descriptionText(text);
    descriptionItem->setData(descriptionText.left(160).append((descriptionText.size() > 160) ? "..." : ""), Qt::DisplayRole);
    descriptionItem->setData(descriptionText, ctkErrorLogModel::DescriptionTextRole);
    descriptionItem->setEditable(false);
    itemList << descriptionItem;

    d->StandardItemModel.invisibleRootItem()->appendRow(itemList);
    }
  else
    {
    // Retrieve description associated with last row
    QModelIndex lastRowDescriptionIndex =
        d->StandardItemModel.index(d->StandardItemModel.rowCount() - 1, ctkErrorLogModel::DescriptionColumn);

    QStringList updatedDescription;
    updatedDescription << lastRowDescriptionIndex.data(ctkErrorLogModel::DescriptionTextRole).toString();
    updatedDescription << text;

    d->StandardItemModel.setData(lastRowDescriptionIndex, updatedDescription.join("\n"),
                                 ctkErrorLogModel::DescriptionTextRole);

    // Append '...' to displayText if needed
    QString displayText = lastRowDescriptionIndex.data().toString();
    if (!displayText.endsWith("..."))
      {
      d->StandardItemModel.setData(lastRowDescriptionIndex, displayText.append("..."), Qt::DisplayRole);
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
void ctkErrorLogModel::clear()
{
  Q_D(ctkErrorLogModel);
  d->StandardItemModel.invisibleRootItem()->removeRows(0, d->StandardItemModel.rowCount());
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::filterEntry(const ctkErrorLogLevel::LogLevels& logLevel,
                                   bool disableFilter)
{
  Q_D(ctkErrorLogModel);

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
ctkErrorLogLevel::LogLevels ctkErrorLogModel::logLevelFilter()const
{
  Q_D(const ctkErrorLogModel);
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
bool ctkErrorLogModel::logEntryGrouping()const
{
  Q_D(const ctkErrorLogModel);
  return d->LogEntryGrouping;
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setLogEntryGrouping(bool value)
{
  Q_D(ctkErrorLogModel);
  d->LogEntryGrouping = value;
}

//------------------------------------------------------------------------------
bool ctkErrorLogModel::asynchronousLogging()const
{
  Q_D(const ctkErrorLogModel);
  return d->AsynchronousLogging;
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setAsynchronousLogging(bool value)
{
  Q_D(ctkErrorLogModel);
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
QString ctkErrorLogModel::filePath()const
{
  Q_D(const ctkErrorLogModel);
  return d->FileLogger.filePath();
}

// --------------------------------------------------------------------------
void ctkErrorLogModel::setFilePath(const QString& filePath)
{
  Q_D(ctkErrorLogModel);
  return d->FileLogger.setFilePath(filePath);
}

// --------------------------------------------------------------------------
int ctkErrorLogModel::numberOfFilesToKeep()const
{
  Q_D(const ctkErrorLogModel);
  return d->FileLogger.numberOfFilesToKeep();
}

// --------------------------------------------------------------------------
void ctkErrorLogModel::setNumberOfFilesToKeep(int value)
{
  Q_D(ctkErrorLogModel);
  return d->FileLogger.setNumberOfFilesToKeep(value);
}

// --------------------------------------------------------------------------
bool ctkErrorLogModel::fileLoggingEnabled()const
{
  Q_D(const ctkErrorLogModel);
  return d->FileLogger.enabled();
}

// --------------------------------------------------------------------------
void ctkErrorLogModel::setFileLoggingEnabled(bool value)
{
  Q_D(ctkErrorLogModel);
  d->FileLogger.setEnabled(value);
}

// --------------------------------------------------------------------------
QString ctkErrorLogModel::fileLoggingPattern()const
{
  Q_D(const ctkErrorLogModel);
  return d->FileLoggingPattern;
}

// --------------------------------------------------------------------------
void ctkErrorLogModel::setFileLoggingPattern(const QString& value)
{
  Q_D(ctkErrorLogModel);
  d->FileLoggingPattern = value;
}

// --------------------------------------------------------------------------
QVariant ctkErrorLogModel::logEntryData(int row, int column, int role) const
{
  Q_D(const ctkErrorLogModel);
  if (column < 0 || column > Self::MaxColumn
      || row < 0 || row > this->logEntryCount())
    {
    return QVariant();
    }
  QModelIndex rowDescriptionIndex = d->StandardItemModel.index(row, column);
  return rowDescriptionIndex.data(role);
}

// --------------------------------------------------------------------------
QString ctkErrorLogModel::logEntryDescription(int row) const
{
  return this->logEntryData(row, Self::DescriptionColumn, Self::DescriptionTextRole).toString();
}

// --------------------------------------------------------------------------
int ctkErrorLogModel::logEntryCount()const
{
  Q_D(const ctkErrorLogModel);
  return d->StandardItemModel.rowCount();
}
