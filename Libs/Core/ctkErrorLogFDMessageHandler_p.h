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
#include <QMutex>
#include <QThread>

// CTK includes
#include "ctkErrorLogAbstractMessageHandler.h"
#include "ctkErrorLogTerminalOutput.h"

// STD includes
#include <cstdio>

class ctkErrorLogFDMessageHandler;

// --------------------------------------------------------------------------
// ctkFDHandler

// --------------------------------------------------------------------------
/// \ingroup Core
class ctkFDHandler : public QThread
{
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
public:
  typedef ctkFDHandler Self;

  ctkFDHandler(ctkErrorLogFDMessageHandler* messageHandler,
               ctkErrorLogLevel::LogLevel logLevel,
               ctkErrorLogTerminalOutput::TerminalOutput terminalOutput);
  virtual ~ctkFDHandler();

  /// Enable/Disable the handler.
  void setEnabled(bool value);

  /// Return if the handler is enabled. This methods is thread-safe.
  bool enabled()const;

  FILE* terminalOutputFile();

protected:
  void setupPipe();

  void run();

private:
  ctkErrorLogFDMessageHandler * MessageHandler;
  ctkErrorLogLevel::LogLevel LogLevel;

  ctkErrorLogTerminalOutput::TerminalOutput TerminalOutput;

  int    SavedFDNumber;
  fpos_t SavedFDPos;

  int          Pipe[2]; // 0: Read, 1: Write

  mutable QMutex EnableMutex;
  bool Enabled;
};


#endif
