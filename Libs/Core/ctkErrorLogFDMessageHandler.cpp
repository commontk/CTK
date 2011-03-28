/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QDir>
#include <QFile>

// CTK includes
#include "ctkErrorLogFDMessageHandler.h"
#include "ctkErrorLogFDMessageHandler_p.h"

// STD includes
#include <cstdio>
#ifdef Q_OS_WIN32
# include <io.h>
#else
# include <unistd.h>
#endif

// --------------------------------------------------------------------------
// ctkFDHandler methods

// --------------------------------------------------------------------------
ctkFDHandler::ctkFDHandler(ctkErrorLogFDMessageHandler* messageHandler,
                           ctkErrorLogModel::LogLevel logLevel,
                           int fileDescriptorNumber)
{
  this->MessageHandler = messageHandler;
  this->LogLevel = logLevel;
  this->FDNumber = fileDescriptorNumber;
  this->SavedFDNumber = 0;
  this->FD = 0;
  this->Enabled = false;

  const QString outputFileTemplateName =  QDir::tempPath()
    + QDir::separator () + "ctkFDHandler-%1.XXXXXX.txt";
  this->OutputFile.setFileTemplate(outputFileTemplateName.arg(this->FDNumber));

  connect(&this->OutputFileWatcher, SIGNAL(fileChanged(const QString&)),
          SLOT(outputFileChanged(const QString&)));
}

// --------------------------------------------------------------------------
ctkErrorLogFDMessageHandler::~ctkErrorLogFDMessageHandler()
{
}

// --------------------------------------------------------------------------
FILE* ctkFDHandler::fileDescriptorFromNumber(int fdNumber)
{
  Q_ASSERT(fdNumber == 1 /* stdout*/ || fdNumber == 2 /*stderr*/);
  if (fdNumber == 1)
    {
    return stdout;
    }
  else if (fdNumber == 2)
    {
    return stderr;
    }
  return 0;
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
    // Flush (stdout|stderr) so that any buffered messages are delivered
    fflush(Self::fileDescriptorFromNumber(this->FDNumber));

    // Save position of current standard output
    fgetpos(Self::fileDescriptorFromNumber(this->FDNumber), &this->SavedFDPos);
#ifdef Q_OS_WIN32
    this->SavedFDNumber = _dup(_fileno(Self::fileDescriptorFromNumber(this->FDNumber)));
#else
    this->SavedFDNumber = dup(fileno(Self::fileDescriptorFromNumber(this->FDNumber)));
#endif

    // Open and close the OutputFile so that the unique filename is created
    if (!this->OutputFile.exists())
      {
      this->OutputFile.open();
      this->OutputFile.close();
      }

    //qDebug() << "ctkFDHandler - OutputFile" << this->OutputFile.fileName();

    if ((this->FD = freopen(this->OutputFile.fileName().toLatin1(),
                            "w",
                            Self::fileDescriptorFromNumber(this->FDNumber))) == 0)
      {
      // this->SavedFDNumber = 0;
      }

    // Observe the OutputFile for changes
    this->OutputFileWatcher.addPath(this->OutputFile.fileName());
    }
  else
    {
    // Flush stdout or stderr so that any buffered messages are delivered
    fflush(Self::fileDescriptorFromNumber(this->FDNumber));

    // Flush current stream so that any buffered messages are delivered
    fflush(this->FD);

    // Un-observe OutputFile
    this->OutputFileWatcher.removePath(this->OutputFile.fileName());

    // Close file and restore standard output to stdout or stderr - which should be the terminal
#ifdef Q_OS_WIN32
    _dup2(this->SavedFDNumber, _fileno(Self::fileDescriptorFromNumber(this->FDNumber)));
    _close(this->SavedFDNumber);
#else
    dup2(this->SavedFDNumber, fileno(Self::fileDescriptorFromNumber(this->FDNumber)));
    close(this->SavedFDNumber);
#endif
    clearerr(Self::fileDescriptorFromNumber(this->FDNumber));
    fsetpos(Self::fileDescriptorFromNumber(this->FDNumber), &this->SavedFDPos);
    }

  this->Enabled = value;
}

// --------------------------------------------------------------------------
void ctkFDHandler::outputFileChanged(const QString & path)
{
  QFile file(path);
  if (!file.open(QFile::ReadOnly))
    {
    qCritical() << "ctkFDHandler - Failed to open file" << path;
    return;
    }

  QTextStream stream(&file);
  while (!stream.atEnd())
    {
    Q_ASSERT(this->MessageHandler->errorLogModel());
    this->MessageHandler->errorLogModel()->addEntry(
          this->LogLevel, this->MessageHandler->handlerPrettyName(), qPrintable(stream.readLine()));
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
  d->StdOutFDHandler = new ctkFDHandler(this, ctkErrorLogModel::Info, 1 /* stdout */);
  d->StdErrFDHandler = new ctkFDHandler(this, ctkErrorLogModel::Critical, 2 /* stderr */);
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
