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
#include <QFile>
#include <QTextStream>

// CTK includes
#include "ctkFileLogger.h"

// --------------------------------------------------------------------------
// ctkFileLoggerPrivate

// --------------------------------------------------------------------------
class ctkFileLoggerPrivate
{
  Q_DECLARE_PUBLIC(ctkFileLogger);
protected:
  ctkFileLogger* const q_ptr;
public:
  ctkFileLoggerPrivate(ctkFileLogger& object);
  ~ctkFileLoggerPrivate();

  void init();

  bool Enabled;
  QString FilePath;
  int NumberOfFilesToKeep;
};

// --------------------------------------------------------------------------
ctkFileLoggerPrivate::ctkFileLoggerPrivate(ctkFileLogger& object)
  : q_ptr(&object)
{
  this->Enabled = true;
  this->NumberOfFilesToKeep = 10;
}

// --------------------------------------------------------------------------
ctkFileLoggerPrivate::~ctkFileLoggerPrivate()
{
}

// --------------------------------------------------------------------------
void ctkFileLoggerPrivate::init()
{
}

// --------------------------------------------------------------------------
// ctkFileLogger

// --------------------------------------------------------------------------
ctkFileLogger::ctkFileLogger(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkFileLoggerPrivate(*this))
{
  Q_D(ctkFileLogger);
  d->init();
}

// --------------------------------------------------------------------------
ctkFileLogger::~ctkFileLogger()
{
}

// --------------------------------------------------------------------------
bool ctkFileLogger::enabled()const
{
  Q_D(const ctkFileLogger);
  return d->Enabled;
}

// --------------------------------------------------------------------------
void ctkFileLogger::setEnabled(bool value)
{
  Q_D(ctkFileLogger);
  d->Enabled = value;
}

// --------------------------------------------------------------------------
QString ctkFileLogger::filePath()const
{
  Q_D(const ctkFileLogger);
  return d->FilePath;
}

// --------------------------------------------------------------------------
void ctkFileLogger::setFilePath(const QString& filePath)
{
  Q_D(ctkFileLogger);
  d->FilePath = filePath;
}

// --------------------------------------------------------------------------
int ctkFileLogger::numberOfFilesToKeep()const
{
  Q_D(const ctkFileLogger);
  return d->NumberOfFilesToKeep;
}

// --------------------------------------------------------------------------
void ctkFileLogger::setNumberOfFilesToKeep(int value)
{
  Q_D(ctkFileLogger);
  d->NumberOfFilesToKeep = value;
}

// --------------------------------------------------------------------------
void ctkFileLogger::logMessage(const QString& msg)
{
  Q_D(ctkFileLogger);
  if (!d->Enabled)
    {
    return;
    }
  QFile f(d->FilePath);
  if (!f.open(QFile::Append))
    {
    return;
    }
  QTextStream s(&f);
  s << msg << endl;
  f.close();
}

