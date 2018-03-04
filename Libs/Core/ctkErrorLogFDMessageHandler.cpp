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
#include <QFile>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkErrorLogFDMessageHandler.h"
#include "ctkErrorLogFDMessageHandler_p.h"
#include "ctkUtils.h"

// STD includes
#include <cstdio>
#ifdef Q_OS_WIN32
# include <fcntl.h>  // For _O_TEXT
# include <io.h>     // For _pipe, _dup and _dup2
#else
# include <unistd.h> // For pipe, dup and dup2
#endif

// --------------------------------------------------------------------------
// ctkFDHandler methods
// See http://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string
// and http://stackoverflow.com/questions/955962/how-to-buffer-stdout-in-memory-and-write-it-from-a-dedicated-thread

// --------------------------------------------------------------------------
ctkFDHandler::ctkFDHandler(ctkErrorLogFDMessageHandler* messageHandler,
                           ctkErrorLogLevel::LogLevel logLevel,
                           ctkErrorLogTerminalOutput::TerminalOutput terminalOutput)
{
  this->MessageHandler = messageHandler;
  this->LogLevel = logLevel;
  this->TerminalOutput = terminalOutput;
  this->SavedFDNumber = 0;
  this->Pipe[0] = -1;
  this->Pipe[1] = -1;
  this->Enabled = false;
}

// --------------------------------------------------------------------------
ctkFDHandler::~ctkFDHandler()
{
}

// --------------------------------------------------------------------------
void ctkFDHandler::setupPipe()
{
#ifdef Q_OS_WIN32
  int status = _pipe(this->Pipe, 65536, _O_TEXT);
#else
  int status = pipe(this->Pipe);
#endif
  if (status != 0)
    {
    qCritical().nospace() << "ctkFDHandler - Failed to create pipe !";
    return;
    }
}

// --------------------------------------------------------------------------
FILE* ctkFDHandler::terminalOutputFile()
{
  return this->TerminalOutput == ctkErrorLogTerminalOutput::StandardOutput ? stdout : stderr;
}

// --------------------------------------------------------------------------
void ctkFDHandler::setEnabled(bool value)
{
  if (this->Enabled == value)
    {
    return;
    }

  if (value)
    {
    this->setupPipe();

    // Flush (stdout|stderr) so that any buffered messages are delivered
    fflush(this->terminalOutputFile());

    // Save position of current standard output
    fgetpos(this->terminalOutputFile(), &this->SavedFDPos);
#ifdef Q_OS_WIN32
    this->SavedFDNumber = _dup(_fileno(this->terminalOutputFile()));
    _dup2(this->Pipe[1], _fileno(this->terminalOutputFile()));
    _close(this->Pipe[1]);
#else
    this->SavedFDNumber = dup(fileno(this->terminalOutputFile()));
    dup2(this->Pipe[1], fileno(this->terminalOutputFile()));
    close(this->Pipe[1]);
#endif

    // Start polling thread
    this->Enabled = true;
    this->start();
    }
  else
    {
    // Print one character to "unblock" the read function associated with the polling thread
#ifdef Q_OS_WIN32
    int unused = _write(_fileno(this->terminalOutputFile()), "\n", 1);
#else
    ssize_t unused = write(fileno(this->terminalOutputFile()), "\n", 1);
#endif
    Q_UNUSED(unused);

    // Flush stdout or stderr so that any buffered messages are delivered
    fflush(this->terminalOutputFile());

    // Stop polling thread
    {
      QMutexLocker locker(&this->EnableMutex);
      this->Enabled = false;
    }

    QString newline("\n");
#ifdef Q_OS_WIN32
    unused = _write(_fileno(this->terminalOutputFile()), qPrintable(newline), newline.size());
#else
    unused = write(fileno(this->terminalOutputFile()), qPrintable(newline), newline.size());
#endif
    Q_UNUSED(unused);

    // Wait the polling thread graciously terminates
    this->wait();

    // Close files and restore standard output to stdout or stderr - which should be the terminal
#ifdef Q_OS_WIN32
    _dup2(this->SavedFDNumber, _fileno(this->terminalOutputFile()));
    _close(this->SavedFDNumber);
#else
    dup2(this->SavedFDNumber, fileno(this->terminalOutputFile()));
    close(this->SavedFDNumber);
#endif
    clearerr(this->terminalOutputFile());
    fsetpos(this->terminalOutputFile(), &this->SavedFDPos);


#ifdef Q_OS_WIN32
    _close(this->Pipe[0]);
#else
    close(this->Pipe[0]);
#endif

    this->SavedFDNumber = 0;
    }

  ctkErrorLogTerminalOutput * terminalOutput =
      this->MessageHandler->terminalOutput(this->TerminalOutput);
  if(terminalOutput)
    {
    terminalOutput->setFileDescriptor(this->SavedFDNumber);
    }
}

// --------------------------------------------------------------------------
bool ctkFDHandler::enabled()const
{
  QMutexLocker locker(&this->EnableMutex);
  return this->Enabled;
}

// --------------------------------------------------------------------------
void ctkFDHandler::run()
{
  while(true)
    {
    char c = '\0';
    QString line;
    while(c != '\n')
      {
#ifdef Q_OS_WIN32
      int res = _read(this->Pipe[0], &c, 1); // When used with pipe, read() is blocking
#else
      ssize_t res = read(this->Pipe[0], &c, 1); // When used with pipe, read() is blocking
#endif
      if (res == -1)
        {
        break;
        }
      if (c != '\n')
        {
        line += c;
        }
      }

    if (!this->enabled())
      {
      break;
      }

    Q_ASSERT(this->MessageHandler);
    this->MessageHandler->handleMessage(
      ctk::qtHandleToString(QThread::currentThreadId()),
      this->LogLevel,
      this->MessageHandler->handlerPrettyName(),
      ctkErrorLogContext(line),
      line);
    }
}

// --------------------------------------------------------------------------
// ctkErrorLogFDMessageHandlerPrivate

// --------------------------------------------------------------------------
class ctkErrorLogFDMessageHandlerPrivate
{
public:
  ctkErrorLogFDMessageHandlerPrivate();
  ~ctkErrorLogFDMessageHandlerPrivate();

  ctkFDHandler * StdOutFDHandler;
  ctkFDHandler * StdErrFDHandler;
};

// --------------------------------------------------------------------------
// ctkErrorLogFDMessageHandlerPrivate methods

//------------------------------------------------------------------------------
ctkErrorLogFDMessageHandlerPrivate::ctkErrorLogFDMessageHandlerPrivate()
{
}

//------------------------------------------------------------------------------
ctkErrorLogFDMessageHandlerPrivate::~ctkErrorLogFDMessageHandlerPrivate()
{
  delete this->StdOutFDHandler;
  delete this->StdErrFDHandler;
}

//------------------------------------------------------------------------------
// ctkErrorLogFDMessageHandler methods

//------------------------------------------------------------------------------
QString ctkErrorLogFDMessageHandler::HandlerName = QLatin1String("FD");

// --------------------------------------------------------------------------
ctkErrorLogFDMessageHandler::ctkErrorLogFDMessageHandler() :
  Superclass(), d_ptr(new ctkErrorLogFDMessageHandlerPrivate())
{
  Q_D(ctkErrorLogFDMessageHandler);
  d->StdOutFDHandler = new ctkFDHandler(this, ctkErrorLogLevel::Info, ctkErrorLogTerminalOutput::StandardOutput);
  d->StdErrFDHandler = new ctkFDHandler(this, ctkErrorLogLevel::Critical, ctkErrorLogTerminalOutput::StandardError);
}

// --------------------------------------------------------------------------
ctkErrorLogFDMessageHandler::~ctkErrorLogFDMessageHandler()
{
}

// --------------------------------------------------------------------------
QString ctkErrorLogFDMessageHandler::handlerName()const
{
  return ctkErrorLogFDMessageHandler::HandlerName;
}

// --------------------------------------------------------------------------
void ctkErrorLogFDMessageHandler::setEnabledInternal(bool value)
{
  Q_D(ctkErrorLogFDMessageHandler);
  d->StdOutFDHandler->setEnabled(value);
  d->StdErrFDHandler->setEnabled(value);
}
