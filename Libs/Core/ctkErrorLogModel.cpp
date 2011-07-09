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
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMainWindow>
#include <QMetaEnum>
#include <QMetaType>
#include <QPointer>
#include <QStandardItem>
#include <QStatusBar>

// CTK includes
#include "ctkErrorLogModel.h"
#include <ctkPimpl.h>

// --------------------------------------------------------------------------
// ctkErrorLogAbstractMessageHandler methods

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandler::ctkErrorLogAbstractMessageHandler()
  : Enabled(false)
{
}

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandler::~ctkErrorLogAbstractMessageHandler()
{
}

// --------------------------------------------------------------------------
ctkErrorLogModel* ctkErrorLogAbstractMessageHandler::errorLogModel()const
{
  return this->ErrorLogModel.data();
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setErrorLogModel(ctkErrorLogModel * newErrorLogModel)
{
  this->ErrorLogModel = QPointer<ctkErrorLogModel>(newErrorLogModel);
}

// --------------------------------------------------------------------------
QString ctkErrorLogAbstractMessageHandler::handlerPrettyName()const
{
  if (this->HandlerPrettyName.isEmpty())
    {
    return this->handlerName();
    }
  else
    {
    return this->HandlerPrettyName;
    }
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setHandlerPrettyName(const QString& newHandlerPrettyName)
{
  this->HandlerPrettyName = newHandlerPrettyName;
}

// --------------------------------------------------------------------------
bool ctkErrorLogAbstractMessageHandler::enabled()const
{
  return this->Enabled;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setEnabled(bool value)
{
  if (value == this->Enabled)
    {
    return;
    }
  this->setEnabledInternal(value);
  this->Enabled = value;
}


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

  QStandardItemModel StandardItemModel;

  QHash<QString, ctkErrorLogAbstractMessageHandler*> RegisteredHandlers;

  ctkErrorLogModel::LogLevels CurrentLogLevelFilter;

  bool LogEntryGrouping;

  bool AddingEntry;

  int TerminalOutputEnabled;

};

// --------------------------------------------------------------------------
// ctkErrorLogModelPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::ctkErrorLogModelPrivate(ctkErrorLogModel& object)
  : q_ptr(&object)
{
  this->AddingEntry = false;
  this->TerminalOutputEnabled = false;
}

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::~ctkErrorLogModelPrivate()
{
  this->LogEntryGrouping = false;
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
  msgHandler->setErrorLogModel(this);
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
bool ctkErrorLogModel::terminalOutputEnabled()const
{
  Q_D(const ctkErrorLogModel);
  return d->TerminalOutputEnabled;
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::setTerminalOutputEnabled(bool enabled)
{
  Q_D(ctkErrorLogModel);
  d->TerminalOutputEnabled = enabled;
}

//------------------------------------------------------------------------------
QString ctkErrorLogModel::logLevelAsString(LogLevel logLevel)const
{
  QMetaEnum logLevelEnum = this->metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);
  return QLatin1String(logLevelEnum.valueToKey(logLevel));
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::addEntry(ctkErrorLogModel::LogLevel logLevel,
                                const QString& origin, const char* text)
{
  Q_D(ctkErrorLogModel);

  if (d->AddingEntry)
    {
//    QFile f("/tmp/ctkErrorLogModel-AddingEntry-true.txt");
//    f.open(QFile::Append);
//    QTextStream s(&f);
//    s << "text=>" << text << "\n";
//    s << "\tlogLevel:" << qPrintable(this->logLevelAsString(logLevel)) << "\n";
//    s << "\torigin:" << qPrintable(origin) << "\n";
//    s << "\ttext:" << text << "\n";
//    f.close();
    return;
    }

  d->AddingEntry = true;

  QString timeFormat("dd.MM.yyyy hh:mm:ss");
  QDateTime currentDateTime = QDateTime::currentDateTime();

  bool groupEntry = false;
  if (d->LogEntryGrouping)
    {
    // Retrieve logLevel associated with last row
    QModelIndex lastRowLogLevelIndex =
        d->StandardItemModel.index(d->StandardItemModel.rowCount() - 1, ctkErrorLogModel::LogLevelColumn);

    bool logLevelMatched = this->logLevelAsString(logLevel) == lastRowLogLevelIndex.data().toString();

    // Retrieve origin associated with last row
    QModelIndex lastRowOriginIndex =
        d->StandardItemModel.index(d->StandardItemModel.rowCount() - 1, ctkErrorLogModel::OriginColumn);

    bool originMatched = origin == lastRowOriginIndex.data().toString();

    // Retrieve time associated with last row
    QModelIndex lastRowTimeIndex =
            d->StandardItemModel.index(d->StandardItemModel.rowCount() - 1, ctkErrorLogModel::TimeColumn);
    QDateTime lastRowDateTime = QDateTime::fromString(lastRowTimeIndex.data().toString(), timeFormat);

    int groupingIntervalInMsecs = 1000;
    bool withinGroupingInterval = lastRowDateTime.time().msecsTo(currentDateTime.time()) <= groupingIntervalInMsecs;

    groupEntry = logLevelMatched && originMatched && withinGroupingInterval;
    }

  if (!groupEntry)
    {
    QList<QStandardItem*> itemList;

    // Time item
    QStandardItem * timeItem = new QStandardItem(currentDateTime.toString(timeFormat));
    timeItem->setEditable(false);
    itemList << timeItem;

    // LogLevel item
    QStandardItem * logLevelItem = new QStandardItem(this->logLevelAsString(logLevel));
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
    updatedDescription << QLatin1String(text);

    d->StandardItemModel.setData(lastRowDescriptionIndex, updatedDescription.join("\n"),
                                 ctkErrorLogModel::DescriptionTextRole);

    // Append '...' to displayText if needed
    QString displayText = lastRowDescriptionIndex.data().toString();
    if (!displayText.endsWith("..."))
      {
      d->StandardItemModel.setData(lastRowDescriptionIndex, displayText.append("..."), Qt::DisplayRole);
      }
    }

  if (d->TerminalOutputEnabled)
    {
    QStringList savedMsgHandlerEnabled = this->msgHandlerEnabled();

    this->disableAllMsgHandler();
    if (logLevel <= ctkErrorLogModel::Info)
      {
      std::cout << text << std::endl;
      }
    else
      {
      std::cerr << text << std::endl;
      }
    this->setMsgHandlerEnabled(savedMsgHandlerEnabled);
    }

  d->AddingEntry = false;
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::clear()
{
  Q_D(ctkErrorLogModel);
  d->StandardItemModel.invisibleRootItem()->removeRows(0, d->StandardItemModel.rowCount());
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::filterEntry(const LogLevels& logLevel, bool disableFilter)
{
  Q_D(ctkErrorLogModel);

  QStringList patterns;
  if (!this->filterRegExp().pattern().isEmpty())
    {
    patterns << this->filterRegExp().pattern().split("|");
    }
  patterns.removeAll(this->logLevelAsString(Self::None));

//  foreach(QString s, patterns)
//    {
//    std::cout << "pattern:" << qPrintable(s) << std::endl;
//    }

  QMetaEnum logLevelEnum = this->metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);

  // Loop over enum values and append associated name to 'patterns' if
  // it has been specified within 'logLevel'
  for (int i = 1; i < logLevelEnum.keyCount(); ++i)
    {
    int aLogLevel = logLevelEnum.value(i);
    if (logLevel & aLogLevel)
      {
      QString logLevelAsString = this->logLevelAsString(static_cast<Self::LogLevel>(aLogLevel));
      if (!disableFilter)
        {
        patterns << logLevelAsString;
        d->CurrentLogLevelFilter |= static_cast<Self::LogLevels>(aLogLevel);
        }
      else
        {
        patterns.removeAll(logLevelAsString);
        d->CurrentLogLevelFilter ^= static_cast<Self::LogLevels>(aLogLevel);
        }
      }
    }

  if (patterns.isEmpty())
    {
    // If there are no patterns, let's filter with the None level so that
    // all entries are filtered out.
    patterns << this->logLevelAsString(Self::None);
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
ctkErrorLogModel::LogLevels ctkErrorLogModel::logLevelFilter()const
{
  QMetaEnum logLevelEnum = this->metaObject()->enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);

  Self::LogLevels filter = Self::Unknown;
  foreach(const QString& filterAsString, this->filterRegExp().pattern().split("|"))
    {
    filter |= static_cast<Self::LogLevels>(logLevelEnum.keyToValue(filterAsString.toLatin1()));
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
