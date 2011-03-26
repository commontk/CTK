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

#ifndef __ctkErrorLogFDMessageHandler_p_h
#define __ctkErrorLogFDMessageHandler_p_h

// Qt includes
#include <QObject>
#include <QFileSystemWatcher>
#include <QTemporaryFile>

// CTK includes
#include "ctkErrorLogModel.h"

// STD includes
#include <cstdio>

class ctkErrorLogFDMessageHandler;

// --------------------------------------------------------------------------
class ctkFDHandler : public QObject
{
  Q_OBJECT
public:
  typedef ctkFDHandler Self;

  ctkFDHandler(ctkErrorLogFDMessageHandler* messageHandler,
               ctkErrorLogModel::LogLevel logLevel,
               int fileDescriptorNumber);

  void setEnabled(bool value);

  static FILE* fileDescriptorFromNumber(int fdNumber);

public slots:
  void outputFileChanged(const QString & path);

private:
  ctkErrorLogFDMessageHandler * MessageHandler;
  ctkErrorLogModel::LogLevel LogLevel;

  /// OutputFile where either stdout or stderr is redirected
  QTemporaryFile     OutputFile;

  /// The fileWatcher will emit the signal 'fileChanged()' each time the outputFile changed.
  QFileSystemWatcher OutputFileWatcher;

  int    FDNumber;
  int    SavedFDNumber;
  fpos_t SavedFDPos;
  FILE*  FD;

  bool Enabled;
};


#endif
