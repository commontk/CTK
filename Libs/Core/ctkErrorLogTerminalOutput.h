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

#ifndef CTKERRORLOGTERMINALOUTPUT_H
#define CTKERRORLOGTERMINALOUTPUT_H

// Qt includes
#include <QScopedPointer>
#include <QObject>

// CTK includes
#include <ctkCoreExport.h>

//------------------------------------------------------------------------------
class ctkErrorLogTerminalOutputPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogTerminalOutput : public QObject
{
  Q_OBJECT
  Q_FLAGS(TerminalOutputs)

public:
  ctkErrorLogTerminalOutput();
  virtual ~ctkErrorLogTerminalOutput();

  enum TerminalOutput
    {
    None            = 0x0,
    StandardError   = 0x1,
    StandardOutput  = 0x2,
    All             = StandardError | StandardOutput
    };
  Q_DECLARE_FLAGS(TerminalOutputs, TerminalOutput)

  bool enabled()const;
  void setEnabled(bool value);

  int fileDescriptor()const;
  void setFileDescriptor(int fd);

  void output(const QString& text);

protected:
  QScopedPointer<ctkErrorLogTerminalOutputPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogTerminalOutput)
  Q_DISABLE_COPY(ctkErrorLogTerminalOutput)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkErrorLogTerminalOutput::TerminalOutputs)

#endif // CTKERRORLOGTERMINALOUTPUT_H
