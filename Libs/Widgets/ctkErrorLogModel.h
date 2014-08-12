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
#include <QSortFilterProxyModel>

// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkErrorLogLevel.h"
#include "ctkErrorLogTerminalOutput.h"

//------------------------------------------------------------------------------
class ctkErrorLogAbstractMessageHandler;
class ctkErrorLogModelPrivate;
struct ctkErrorLogContext;

//------------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkErrorLogModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(bool logEntryGrouping READ logEntryGrouping WRITE setLogEntryGrouping)
  Q_PROPERTY(ctkErrorLogTerminalOutput::TerminalOutputs terminalOutputs READ terminalOutputs WRITE setTerminalOutputs)
  Q_PROPERTY(bool asynchronousLogging READ asynchronousLogging WRITE  setAsynchronousLogging)
  Q_PROPERTY(QString filePath READ filePath WRITE  setFilePath)
  Q_PROPERTY(int numberOfFilesToKeep READ numberOfFilesToKeep WRITE  setNumberOfFilesToKeep)
  Q_PROPERTY(bool fileLoggingEnabled READ fileLoggingEnabled WRITE  setFileLoggingEnabled)
  Q_PROPERTY(QString fileLoggingPattern READ fileLoggingPattern WRITE setFileLoggingPattern)
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

  /// Return if messages are both printed into the terminal and added to ctkErrorLogModel.
  /// \note If TerminalOutput::None is returned, message will only be added to the model.
  ctkErrorLogTerminalOutput::TerminalOutputs terminalOutputs()const;

  /// Set terminal output mode
  /// \sa terminalOutputs()
  /// \sa TerminalOutput
  void setTerminalOutputs(const ctkErrorLogTerminalOutput::TerminalOutputs& terminalOutput);

  ctkErrorLogLevel::LogLevels logLevelFilter()const;

  void filterEntry(const ctkErrorLogLevel::LogLevels& logLevel = ctkErrorLogLevel::Unknown, bool disableFilter = false);

  bool logEntryGrouping()const;
  void setLogEntryGrouping(bool value);

  bool asynchronousLogging()const;
  void setAsynchronousLogging(bool value);

  QString filePath()const;
  void setFilePath(const QString& filePath);

  int numberOfFilesToKeep()const;
  void setNumberOfFilesToKeep(int value);

  bool fileLoggingEnabled()const;
  void setFileLoggingEnabled(bool value);

  QString fileLoggingPattern()const;
  void setFileLoggingPattern(const QString& value);

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
                ctkErrorLogLevel::LogLevel logLevel, const QString& origin,
                const ctkErrorLogContext &context, const QString& text);

Q_SIGNALS:
  void logLevelFilterChanged();

  /// \sa addEntry()
  void entryAdded(ctkErrorLogLevel::LogLevel logLevel);

protected:
  QScopedPointer<ctkErrorLogModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogModel)
  Q_DISABLE_COPY(ctkErrorLogModel)
};

#endif
