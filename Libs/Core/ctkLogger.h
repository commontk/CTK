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

#ifndef __ctkLogger_h
#define __ctkLogger_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>
#include "ctkCoreExport.h"
#include "ctkErrorLogLevel.h"

class ctkLoggerPrivate;

/// \ingroup Core
class CTK_CORE_EXPORT ctkLogger : public QObject
{
  Q_OBJECT
  Q_PROPERTY(ctkErrorLogLevel::LogLevel level READ logLevel WRITE setLogLevel);
public:
  typedef QObject Superclass;

  /// Default level is ctkErrorLogLevel::LogLevel::Warning
  explicit ctkLogger(QString name, QObject* parent = 0);
  virtual ~ctkLogger();

  Q_INVOKABLE void trace(const QString& s);
  Q_INVOKABLE void debug(const QString& s);
  Q_INVOKABLE void info(const QString& s);
  Q_INVOKABLE void warn(const QString& s);
  Q_INVOKABLE void error(const QString& s);
  Q_INVOKABLE void fatal(const QString& s);

  void setLogLevel(const ctkErrorLogLevel::LogLevel& level);
  ctkErrorLogLevel::LogLevel logLevel() const;

protected:
  QScopedPointer<ctkLoggerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkLogger);
  Q_DISABLE_COPY(ctkLogger);
};

#endif
