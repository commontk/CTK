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

#include "ctkErrorLogTerminalOutput.h"

// Qt includes
#include <QMutex>
#include <QString>

// STD includes
#include <cstdio> // For _fileno or fileno
#ifdef _MSC_VER
# include <io.h> // For _write()
#else
# include <unistd.h>
#endif

// --------------------------------------------------------------------------
// ctkErrorLogTerminalOutputPrivate

// --------------------------------------------------------------------------
class ctkErrorLogTerminalOutputPrivate
{
public:
  ctkErrorLogTerminalOutputPrivate();
  ~ctkErrorLogTerminalOutputPrivate();

  bool Enabled;
  mutable QMutex EnableMutex;

  int FD;
  mutable QMutex OutputMutex;
};

// --------------------------------------------------------------------------
ctkErrorLogTerminalOutputPrivate::ctkErrorLogTerminalOutputPrivate()
  : Enabled(false)
{
#ifdef Q_OS_WIN32
  this->FD = _fileno(stdout);
#else
  this->FD = fileno(stdout);
#endif
}

// --------------------------------------------------------------------------
ctkErrorLogTerminalOutputPrivate::~ctkErrorLogTerminalOutputPrivate()
{
}

// --------------------------------------------------------------------------
// ctkErrorLogTerminalOutput methods

// --------------------------------------------------------------------------
ctkErrorLogTerminalOutput::ctkErrorLogTerminalOutput()
  : d_ptr(new ctkErrorLogTerminalOutputPrivate)
{
}

// --------------------------------------------------------------------------
ctkErrorLogTerminalOutput::~ctkErrorLogTerminalOutput()
{
}

// --------------------------------------------------------------------------
bool ctkErrorLogTerminalOutput::enabled()const
{
  Q_D(const ctkErrorLogTerminalOutput);
  QMutexLocker locker(&d->EnableMutex);
  return d->Enabled;
}

// --------------------------------------------------------------------------
void ctkErrorLogTerminalOutput::setEnabled(bool value)
{
  Q_D(ctkErrorLogTerminalOutput);
  QMutexLocker locker(&d->EnableMutex);
  d->Enabled = value;
}

// --------------------------------------------------------------------------
int ctkErrorLogTerminalOutput::fileDescriptor()const
{
  Q_D(const ctkErrorLogTerminalOutput);
  QMutexLocker locker(&d->OutputMutex);
  return d->FD;
}

// --------------------------------------------------------------------------
void ctkErrorLogTerminalOutput::setFileDescriptor(int fd)
{
  Q_D(ctkErrorLogTerminalOutput);
  QMutexLocker locker(&d->OutputMutex);
  d->FD = fd;
}

// --------------------------------------------------------------------------
void ctkErrorLogTerminalOutput::output(const QString& text)
{
  Q_D(ctkErrorLogTerminalOutput);
  {
    QMutexLocker locker(&d->EnableMutex);
    if (!d->Enabled)
      {
      return;
      }
  }

  {
    QMutexLocker locker(&d->OutputMutex);
    QString textWithNewLine = text + "\n";
#ifdef _MSC_VER
    int res = _write(d->FD, qPrintable(textWithNewLine), textWithNewLine.size());
#else
    ssize_t res = write(d->FD, qPrintable(textWithNewLine), textWithNewLine.size());
#endif
    if (res == -1)
      {
      return;
      }
  }
}
