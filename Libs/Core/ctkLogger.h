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

class ctkLoggerPrivate;

/// \deprecated This class was a wrapper around Log4Qt. Since Log4Qt dependency has been
/// removed, it is advised to use qDebug(), qWarning() and qCritical() instead.
/// \ingroup Core
class CTK_CORE_EXPORT ctkLogger : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  /// Default mode is Off
  explicit ctkLogger(QString name, QObject* parent = 0);
  virtual ~ctkLogger ();

  void debug(const QString& s);
  void info(const QString& s);
  void trace(const QString& s);
  void warn(const QString& s);
  void error(const QString& s);
  void fatal(const QString& s);

protected:
  QScopedPointer<ctkLoggerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkLogger);
  Q_DISABLE_COPY(ctkLogger);
};

#endif
