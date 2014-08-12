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
#include <QMutexLocker>
#include <QThread>
#include <QHash>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkErrorLogStreamMessageHandler.h"
#include "ctkUtils.h"

// STD includes
#include <iostream>
#include <streambuf>
#include <string>

namespace
{

// --------------------------------------------------------------------------
// ctkStreamHandler

//
// See http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
//

// --------------------------------------------------------------------------
class ctkStreamHandler : public std::streambuf
{
public:
  ctkStreamHandler(ctkErrorLogStreamMessageHandler* messageHandler,
                   ctkErrorLogLevel::LogLevel logLevel,
                   std::ostream& stream);

  void setEnabled(bool value);

protected:
  virtual int_type overflow(int_type v);
  virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
  void initBuffer();
  std::string* currentBuffer();

  ctkErrorLogStreamMessageHandler * MessageHandler;
  ctkErrorLogLevel::LogLevel LogLevel;
  ctkErrorLogContext LogContext;

  bool Enabled;

  std::ostream&   Stream;
  std::streambuf* SavedBuffer;
  QHash<Qt::HANDLE, std::string*> StringBuffers;
  QMutex Mutex;
};

// --------------------------------------------------------------------------
// ctkStreamHandler methods

// --------------------------------------------------------------------------
ctkStreamHandler::ctkStreamHandler(ctkErrorLogStreamMessageHandler* messageHandler,
                                   ctkErrorLogLevel::LogLevel logLevel,
                                   std::ostream& stream) :
  MessageHandler(messageHandler), LogLevel(logLevel), Stream(stream)
{
  this->Enabled = false;
}

// --------------------------------------------------------------------------
void ctkStreamHandler::setEnabled(bool value)
{
  if (this->Enabled == value)
    {
    return;
    }

  if (value)
    {
    this->SavedBuffer = this->Stream.rdbuf();
    this->Stream.rdbuf(this);
    }
  else
    {
    // Output anything that is left
//    if (!this->StringBuffer.empty())
//      {
//      Q_ASSERT(this->MessageHandler);
//      this->MessageHandler->handleMessage(this->LogLevel,
//          this->MessageHandler->handlerPrettyName(), this->StringBuffer.c_str());
//      }
    this->Stream.rdbuf(this->SavedBuffer);
    }

  this->Enabled = value;
}

// --------------------------------------------------------------------------
void ctkStreamHandler::initBuffer()
{
  Qt::HANDLE threadId = QThread::currentThreadId();
  if (!this->StringBuffers.contains(threadId))
    {
    this->StringBuffers.insert(threadId, new std::string);
    }
}

// --------------------------------------------------------------------------
std::string* ctkStreamHandler::currentBuffer()
{
  return this->StringBuffers.value(QThread::currentThreadId());
}

// --------------------------------------------------------------------------
std::streambuf::int_type ctkStreamHandler::overflow(std::streambuf::int_type v)
{
  QMutexLocker locker(&this->Mutex);
  this->initBuffer();
  if (v == '\n')
    {
    Q_ASSERT(this->MessageHandler);
    this->MessageHandler->handleMessage(
          ctk::qtHandleToString(QThread::currentThreadId()), this->LogLevel,
          this->MessageHandler->handlerPrettyName(),
          ctkErrorLogContext(this->currentBuffer()->c_str()), this->currentBuffer()->c_str());
    this->currentBuffer()->erase(this->currentBuffer()->begin(), this->currentBuffer()->end());
    }
  else
    {
    *this->currentBuffer() += v;
    }
  return v;
}

// --------------------------------------------------------------------------
std::streamsize ctkStreamHandler::xsputn(const char *p, std::streamsize n)
{
  QMutexLocker locker(&this->Mutex);
  this->initBuffer();
  this->currentBuffer()->append(p, p + n);

  std::string::size_type pos = 0;
  while (pos != std::string::npos)
    {
    pos = this->currentBuffer()->find('\n');
    if (pos != std::string::npos)
      {
      std::string tmp(this->currentBuffer()->begin(), this->currentBuffer()->begin() + pos);
      Q_ASSERT(this->MessageHandler);
      this->MessageHandler->handleMessage(
            ctk::qtHandleToString(QThread::currentThreadId()), this->LogLevel,
            this->MessageHandler->handlerPrettyName(),
            ctkErrorLogContext(tmp.c_str()), tmp.c_str());
      this->currentBuffer()->erase(this->currentBuffer()->begin(), this->currentBuffer()->begin() + pos + 1);
      }
    }
  return n;
}

}

// --------------------------------------------------------------------------
// ctkErrorLogStreamMessageHandlerPrivate

// --------------------------------------------------------------------------
class ctkErrorLogStreamMessageHandlerPrivate
{
public:
  ctkErrorLogStreamMessageHandlerPrivate();
  ~ctkErrorLogStreamMessageHandlerPrivate();

  ctkStreamHandler * CoutStreamHandler;
  ctkStreamHandler * CerrStreamHandler;
};

// --------------------------------------------------------------------------
// ctkErrorLogStreamMessageHandlerPrivate methods

//------------------------------------------------------------------------------
ctkErrorLogStreamMessageHandlerPrivate::ctkErrorLogStreamMessageHandlerPrivate()
{
}

//------------------------------------------------------------------------------
ctkErrorLogStreamMessageHandlerPrivate::~ctkErrorLogStreamMessageHandlerPrivate()
{
  delete this->CoutStreamHandler;
  delete this->CerrStreamHandler;
}

//------------------------------------------------------------------------------
// ctkErrorLogStreamMessageHandler methods

//------------------------------------------------------------------------------
QString ctkErrorLogStreamMessageHandler::HandlerName = QLatin1String("Stream");

// --------------------------------------------------------------------------
ctkErrorLogStreamMessageHandler::ctkErrorLogStreamMessageHandler() :
  Superclass(), d_ptr(new ctkErrorLogStreamMessageHandlerPrivate())
{
  Q_D(ctkErrorLogStreamMessageHandler);
  d->CoutStreamHandler = new ctkStreamHandler(this, ctkErrorLogLevel::Info, std::cout);
  d->CerrStreamHandler = new ctkStreamHandler(this, ctkErrorLogLevel::Critical, std::cerr);
}

// --------------------------------------------------------------------------
ctkErrorLogStreamMessageHandler::~ctkErrorLogStreamMessageHandler()
{
}

// --------------------------------------------------------------------------
QString ctkErrorLogStreamMessageHandler::handlerName()const
{
  return ctkErrorLogStreamMessageHandler::HandlerName;
}

// --------------------------------------------------------------------------
void ctkErrorLogStreamMessageHandler::setEnabledInternal(bool value)
{
  Q_D(ctkErrorLogStreamMessageHandler);
  d->CoutStreamHandler->setEnabled(value);
  d->CerrStreamHandler->setEnabled(value);
}
