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

class ctkErrorLogModel;
class ctkErrorLogModelPrivate;
class QStandardItemModel;

#include <iostream>

//------------------------------------------------------------------------------
class CTK_CORE_EXPORT ctkErrorLogAbstractMessageHandler
{
public:
  /// Disabled by default.
  ctkErrorLogAbstractMessageHandler();
  virtual ~ctkErrorLogAbstractMessageHandler();

  ctkErrorLogModel * errorLogModel()const;
  void setErrorLogModel(ctkErrorLogModel * newErrorLogModel);

  virtual QString handlerName()const = 0;

  QString handlerPrettyName()const;

  bool enabled()const;
  void setEnabled(bool value);

protected:
  void setHandlerPrettyName(const QString& newHandlerPrettyName);

  virtual void setEnabledInternal(bool value) = 0;

private:
  QPointer<ctkErrorLogModel> ErrorLogModel;
  bool Enabled;
  QString HandlerPrettyName;
};

//------------------------------------------------------------------------------
class CTK_CORE_EXPORT ctkErrorLogModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_FLAGS(LogLevel)
  Q_PROPERTY(bool logEntryGrouping READ logEntryGrouping WRITE setLogEntryGrouping)
  Q_PROPERTY(bool terminalOutputEnabled READ terminalOutputEnabled WRITE  setTerminalOutputEnabled)
public:
  typedef QSortFilterProxyModel Superclass;
  typedef ctkErrorLogModel Self;
  explicit ctkErrorLogModel(QObject* parentObject = 0);
  virtual ~ctkErrorLogModel();

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

  enum ColumnsIds
    {
    TimeColumn = 0,
    LogLevelColumn,
    OriginColumn,
    DescriptionColumn
    };

  enum ItemDataRole{
    DescriptionTextRole = Qt::UserRole + 1
    };

  bool registerMsgHandler(ctkErrorLogAbstractMessageHandler * msgHandler);

  QStringList msgHandlerNames()const;

  bool msgHandlerEnabled(const QString& handlerName) const;
  void setMsgHandlerEnabled(const QString& handlerName, bool enabled);

  /// Return names of the enabled message handlers
  QStringList msgHandlerEnabled()const;

  /// Enable handler identified by their names
  void setMsgHandlerEnabled(const QStringList& handlerNames);

  void enableAllMsgHandler();
  void disableAllMsgHandler();
  void setAllMsgHandlerEnabled(bool enabled);

  /// Return True messages are both printed into the terminal and added to ctkErrorLogModel.
  bool terminalOutputEnabled()const;

  void setTerminalOutputEnabled(bool enabled);

  QString logLevelAsString(LogLevel logLevel)const;

  void addEntry(LogLevel logLevel, const QString& origin, const char* text);

  void clear();

  void filterEntry(const LogLevels& logLevel = Unknown, bool disableFilter = false);

  LogLevels logLevelFilter()const;

  bool logEntryGrouping()const;
  void setLogEntryGrouping(bool value);

signals:
  void logLevelFilterChanged();

protected:
  QScopedPointer<ctkErrorLogModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogModel);
  Q_DISABLE_COPY(ctkErrorLogModel);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkErrorLogModel::LogLevels)

#endif
