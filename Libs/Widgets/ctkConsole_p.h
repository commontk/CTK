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

#ifndef __ctkConsole_p_h
#define __ctkConsole_p_h

// Qt includes
#include <QTextEdit>

// CTK includes
#include "ctkConsole.h"
#include "ctkWidgetsExport.h"

class CTK_WIDGETS_EXPORT ctkConsolePrivate : public QTextEdit
{
  Q_DECLARE_PUBLIC(ctkConsole);
protected:
  ctkConsole* const q_ptr;
public:

  ctkConsolePrivate(ctkConsole& object);

  void init();

  void keyPressEvent(QKeyEvent* e);
  
  /// Returns the end of the document
  int documentEnd() const;

  void focusOutEvent(QFocusEvent *e);

  void updateCompleterIfVisible();

  /// If there is exactly 1 completion, insert it and hide the completer,
  /// else do nothing.
  void selectCompletion();

  void updateCompleter();
  
  /// Update the contents of the command buffer from the contents of the widget
  void updateCommandBuffer();
  
  /// Replace the contents of the command buffer, updating the display
  void replaceCommandBuffer(const QString& Text);
  
  /// References the buffer where the current un-executed command is stored
  QString& commandBuffer();
  
  /// Implements command-execution
  void internalExecuteCommand();

  void setCompleter(ctkConsoleCompleter* completer);

  /// A custom completer
  QPointer<ctkConsoleCompleter> Completer;

  /// Stores the beginning of the area of interactive input, outside which
  /// changes can't be made to the text edit contents
  int InteractivePosition;

  /// Stores command-history, plus the current command buffer
  QStringList CommandHistory;

  /// Stores the current position in the command-history
  int CommandPosition;
};


#endif

