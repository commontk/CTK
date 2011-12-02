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

#ifndef __ctkErrorLogFDMessageHandler_p_h
#define __ctkErrorLogFDMessageHandler_p_h

// Qt includes
#include <QFile>
#include <QThread>

// CTK includes
#include "ctkErrorLogModel.h"

// STD includes
#include <cstdio>

class ctkErrorLogFDMessageHandler;
class QTextStream;

// --------------------------------------------------------------------------
// ctkFDHandler

// --------------------------------------------------------------------------
/// \ingroup Core
class ctkFDHandler : public QThread
{
  Q_OBJECT
public:
  typedef ctkFDHandler Self;

  ctkFDHandler(ctkErrorLogFDMessageHandler* messageHandler,
               ctkErrorLogLevel::LogLevel logLevel,
               ctkErrorLogModel::TerminalOutput terminalOutput);
  virtual ~ctkFDHandler();

  void setEnabled(bool value);

  FILE* terminalOutputFile();

protected:
  void init();

  void run();

private:
  ctkErrorLogFDMessageHandler * MessageHandler;
  ctkErrorLogLevel::LogLevel LogLevel;

  ctkErrorLogModel::TerminalOutput TerminalOutput;

  int    SavedFDNumber;
  fpos_t SavedFDPos;

  int          Pipe[2]; // 0: Read, 1: Write
  QFile        RedirectionFile;
  QTextStream* RedirectionStream;

  bool Initialized;
  bool Enabled;
};


#endif
