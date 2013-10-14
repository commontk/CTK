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

#ifndef __ctkErrorLogModel_h
#define __ctkErrorLogModel_h

// Qt includes
#include <QPointer>
#include <QSortFilterProxyModel>

// CTK includes
#include "ctkCoreExport.h"

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogLevel : public QObject
{
  Q_OBJECT
  Q_FLAGS(LogLevel)
public:
  ctkErrorLogLevel();

  enum LogLevel
    {
    None     = 0x0,
    Unknown  = 0x1,
    Status   = 0x2,
    Trace    = 0x4,
    Debug    = 0x8,
    Info     = 0x10,
    Warning  = 0x20,
    Error    = 0x40,
    Critical = 0x80,
    Fatal    = 0x100
    };
  Q_DECLARE_FLAGS(LogLevels, LogLevel)

  QString operator ()(LogLevel logLevel);

  static QString logLevelAsString(ctkErrorLogLevel::LogLevel logLevel);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkErrorLogLevel::LogLevels)

//------------------------------------------------------------------------------
class ctkErrorLogTerminalOutputPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogTerminalOutput
{
public:
  ctkErrorLogTerminalOutput();
  virtual ~ctkErrorLogTerminalOutput();

  bool enabled()const;
  void setEnabled(bool value);

  int fileDescriptor()const;
  void setFileDescriptor(int fd);

  void output(const QString& text);

protected:
  QScopedPointer<ctkErrorLogTerminalOutputPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogTerminalOutput);
  Q_DISABLE_COPY(ctkErrorLogTerminalOutput);
};

//------------------------------------------------------------------------------
class ctkErrorLogAbstractMessageHandler;
class ctkErrorLogModelPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_FLAGS(TerminalOutput)
  Q_PROPERTY(bool logEntryGrouping READ logEntryGrouping WRITE setLogEntryGrouping)
  Q_PROPERTY(TerminalOutput terminalOutputs READ terminalOutputs WRITE  setTerminalOutputs)
  Q_PROPERTY(bool asynchronousLogging READ asynchronousLogging WRITE  setAsynchronousLogging)
public:
  typedef QSortFilterProxyModel Superclass;
  typedef ctkErrorLogModel Self;
  explicit ctkErrorLogModel(QObject* parentObject = 0);
  virtual ~ctkErrorLogModel();

  enum ColumnsIds
    {
    TimeColumn = 0,
    ThreadIdColumn,
    LogLevelColumn,
    OriginColumn,
    DescriptionColumn,
    MaxColumn = DescriptionColumn
    };

  enum ItemDataRole{
    DescriptionTextRole = Qt::UserRole + 1
    };

  /// Register a message handler.
  bool registerMsgHandler(ctkErrorLogAbstractMessageHandler * msgHandler);

  QStringList msgHandlerNames()const;

  /// Return True if the handler identified by \a handlerName is enabled
  bool msgHandlerEnabled(const QString& handlerName) const;

  /// Enable a specific handler given its name
  void setMsgHandlerEnabled(const QString& handlerName, bool enabled);

  /// Return names of the enabled message handlers
  QStringList msgHandlerEnabled()const;

  /// Enable handler identified by their names
  void setMsgHandlerEnabled(const QStringList& handlerNames);

  void enableAllMsgHandler();
  void disableAllMsgHandler();
  void setAllMsgHandlerEnabled(bool enabled);

  enum TerminalOutput
    {
    None            = 0x0,
    StandardError   = 0x1,
    StandardOutput  = 0x2,
    All             = StandardError | StandardOutput
    };
  Q_DECLARE_FLAGS(TerminalOutputs, TerminalOutput)

  /// Return if messages are both printed into the terminal and added to ctkErrorLogModel.
  /// \note If TerminalOutput::None is returned, message will only be added to the model.
  TerminalOutputs terminalOutputs()const;

  /// Set terminal output mode
  /// \sa terminalOutputs()
  /// \sa TerminalOutput
  void setTerminalOutputs(const TerminalOutputs& terminalOutput);

  ctkErrorLogLevel::LogLevels logLevelFilter()const;

  void filterEntry(const ctkErrorLogLevel::LogLevels& logLevel = ctkErrorLogLevel::Unknown, bool disableFilter = false);

  bool logEntryGrouping()const;
  void setLogEntryGrouping(bool value);

  bool asynchronousLogging()const;
  void setAsynchronousLogging(bool value);

  /// Return log entry information associated with \a row and \a column.
  /// \internal
  QVariant logEntryData(int row,
                        int column = ctkErrorLogModel::DescriptionColumn,
                        int role = Qt::DisplayRole) const;

  /// Return log entry information associated with Description column.
  /// \sa ctkErrorLogModel::DescriptionColumn, logEntryData()
  Q_INVOKABLE QString logEntryDescription(int row) const;

  /// Return current number of log entries.
  /// \sa clear()
  Q_INVOKABLE int logEntryCount() const;

public Q_SLOTS:

  /// Remove all log entries from model
  void clear();

  /// \sa logEntryGrouping(), asynchronousLogging()
  void addEntry(const QDateTime& currentDateTime, const QString& threadId,
                ctkErrorLogLevel::LogLevel logLevel, const QString& origin, const QString& text);

Q_SIGNALS:
  void logLevelFilterChanged();

  /// \sa addEntry()
  void entryAdded(ctkErrorLogLevel::LogLevel logLevel);

protected:
  QScopedPointer<ctkErrorLogModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogModel);
  Q_DISABLE_COPY(ctkErrorLogModel);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkErrorLogModel::TerminalOutputs)

//------------------------------------------------------------------------------
class ctkErrorLogAbstractMessageHandlerPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogAbstractMessageHandler : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  /// Disabled by default.
  ctkErrorLogAbstractMessageHandler();
  virtual ~ctkErrorLogAbstractMessageHandler();

  virtual QString handlerName()const = 0;

  QString handlerPrettyName()const;

  bool enabled()const;
  void setEnabled(bool value);

  void handleMessage(const QString& threadId, ctkErrorLogLevel::LogLevel logLevel,
                     const QString& origin, const QString& text);

  ctkErrorLogTerminalOutput* terminalOutput(ctkErrorLogModel::TerminalOutput terminalOutputType)const;
  void setTerminalOutput(ctkErrorLogModel::TerminalOutput terminalOutputType,
                         ctkErrorLogTerminalOutput * terminalOutput);

Q_SIGNALS:
  void messageHandled(const QDateTime& currentDateTime, const QString& threadId,
                      ctkErrorLogLevel::LogLevel logLevel, const QString& origin,
                      const QString& text);

protected:
  void setHandlerPrettyName(const QString& newHandlerPrettyName);

  virtual void setEnabledInternal(bool value) = 0;

protected:
  QScopedPointer<ctkErrorLogAbstractMessageHandlerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogAbstractMessageHandler);
  Q_DISABLE_COPY(ctkErrorLogAbstractMessageHandler);
};
#endif
