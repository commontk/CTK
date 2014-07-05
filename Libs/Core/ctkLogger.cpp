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
#include <QDebug>

// CTK includes
#include <ctkLogger.h>

//-----------------------------------------------------------------------------
class ctkLoggerPrivate
{
public:
};

//-----------------------------------------------------------------------------
ctkLogger::ctkLogger(QString name, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkLoggerPrivate)
{
  Q_UNUSED(name);
}

//-----------------------------------------------------------------------------
ctkLogger::~ctkLogger()
{
}

//-----------------------------------------------------------------------------
void ctkLogger::debug(const QString& s)
{
  qDebug().nospace() << qPrintable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::info(const QString& s)
{
  qDebug().nospace() << qPrintable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::trace(const QString& s)
{
  qDebug().nospace() << qPrintable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::warn(const QString& s)
{
  qWarning().nospace() << qPrintable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::error(const QString& s)
{
  qCritical().nospace() << qPrintable(s);
}

//-----------------------------------------------------------------------------
void ctkLogger::fatal(const QString& s)
{
  qCritical().nospace() << qPrintable(s);
}

