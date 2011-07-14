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

// CTK includes
#include "ctkErrorLogStreamMessageHandler.h"

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
                   ctkErrorLogModel::LogLevel logLevel,
                   std::ostream& stream);

  void setEnabled(bool value);

protected:
  virtual int_type overflow(int_type v);
  virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
  ctkErrorLogStreamMessageHandler * MessageHandler;
  ctkErrorLogModel::LogLevel LogLevel;

  bool Enabled;

  std::ostream&   Stream;
  std::streambuf* SavedBuffer;
  std::string     StringBuffer;
};

// --------------------------------------------------------------------------
// ctkStreamHandler methods

// --------------------------------------------------------------------------
ctkStreamHandler::ctkStreamHandler(ctkErrorLogStreamMessageHandler* messageHandler,
                                   ctkErrorLogModel::LogLevel logLevel,
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
    if (!this->StringBuffer.empty())
      {
      Q_ASSERT(this->MessageHandler->errorLogModel());
      this->MessageHandler->errorLogModel()->addEntry(
            this->LogLevel, this->MessageHandler->handlerPrettyName(), this->StringBuffer.c_str());
      }
    this->Stream.rdbuf(this->SavedBuffer);
    }

  this->Enabled = value;
}

// --------------------------------------------------------------------------
std::streambuf::int_type ctkStreamHandler::overflow(std::streambuf::int_type v)
{
  if (v == '\n')
    {
    Q_ASSERT(this->MessageHandler->errorLogModel());
    this->MessageHandler->errorLogModel()->addEntry(
          this->LogLevel, this->MessageHandler->handlerPrettyName(), this->StringBuffer.c_str());
    this->StringBuffer.erase(this->StringBuffer.begin(), this->StringBuffer.end());
    }
  else
    {
    this->StringBuffer += v;
    }
  return v;
}

// --------------------------------------------------------------------------
std::streamsize ctkStreamHandler::xsputn(const char *p, std::streamsize n)
{
  this->StringBuffer.append(p, p + n);

  std::string::size_type pos = 0;
  while (pos != std::string::npos)
    {
    pos = this->StringBuffer.find('\n');
    if (pos != std::string::npos)
      {
      std::string tmp(this->StringBuffer.begin(), this->StringBuffer.begin() + pos);
      Q_ASSERT(this->MessageHandler->errorLogModel());
      this->MessageHandler->errorLogModel()->addEntry(
            this->LogLevel, this->MessageHandler->handlerPrettyName(), tmp.c_str());
      this->StringBuffer.erase(this->StringBuffer.begin(), this->StringBuffer.begin() + pos + 1);
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
  d->CoutStreamHandler = new ctkStreamHandler(this, ctkErrorLogModel::Info, std::cout);
  d->CerrStreamHandler = new ctkStreamHandler(this, ctkErrorLogModel::Critical, std::cerr);
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
