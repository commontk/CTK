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

#ifndef __ctkConsole_p_h
#define __ctkConsole_p_h

// Qt includes
#include <QTextEdit>
#include <QPointer>
#include <QEventLoop>

// CTK includes
#include "ctkConsole.h"
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkConsolePrivate : public QTextEdit
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkConsole);
protected:
  ctkConsole* const q_ptr;
public:

  ctkConsolePrivate(ctkConsole& object);
  typedef QTextEdit Superclass;

  void init();

  static bool isMoveLeftWithinLine(QKeyEvent* e, QTextCursor::MoveOperation &moveOperation, QTextCursor::MoveMode &moveMode);

  virtual void keyPressEvent(QKeyEvent* e);

  void switchToUserInputTextColor(QTextCursor* textCursorToUpdate = 0);
  
  /// Returns the end of the document
  int documentEnd() const;

  virtual void focusOutEvent(QFocusEvent *e);

  virtual void resizeEvent(QResizeEvent * e);

  /// Force the scrollbar to be all the way down
  void scrollToBottom();

  void updateCompleterIfVisible();

  /// If there is exactly 1 completion, insert it and hide the completer,
  /// else do nothing.
  void selectCompletion();

  void setCompleter(ctkConsoleCompleter* completer);

  void updateCompleter();
  
  /// Update the contents of the command buffer from the contents of the widget
  void updateCommandBuffer();
  
  /// Replace the contents of the command buffer, updating the display
  void replaceCommandBuffer(const QString& text);
  
  /// References the buffer where the current un-executed command is stored
  QString& commandBuffer();
  
  /// Implements command-execution
  void internalExecuteCommand();

  void processInput();

  /// Writes the supplied text to the console
  void printString(const QString& text);

  /// Updates the current command.
  /// Unlike printMessage(), this will affect the current command being typed.
  void printCommand(const QString& cmd);

  /// Puts out an input accepting promp using either ps1 or ps2.
  /// ps2 will be used if MultilineStatement is True
  /// \sa ctkConsole::ps1(), ctkConsole::ps2()
  void promptForInput(const QString& indent = QString());

  /// Puts out an input accepting prompt.
  /// It is recommended that one uses prompt instead of printMessage() to print
  /// an input prompt since this call ensures that the prompt is shown on a new
  /// line.
  void prompt(const QString& text);

  /// Print welcome message
  virtual void printWelcomeMessage();

public Q_SLOTS:

  /// Inserts the given completion string at the cursor.  This will replace
  /// the current word that the cursor is touching with the given text.
  /// Determines the word using QTextCursor::StartOfWord, EndOfWord.
  void insertCompletion(const QString& text);

  /// Print a message
  /// \sa ctkConsole::outputTextColor
  void printOutputMessage(const QString& text);

  /// Print a message
  /// \sa ctkConsole::errorTextColor
  void printErrorMessage(const QString& text);

  /// Update the value of ScrollbarAtBottom given the current position of the scollbar
  void onScrollBarValueChanged(int value);

  /// Ensure the interactive line is visible (even when it is split on 2 lines)
  /// \sa onScrollBarValueChanged()
  void onTextChanged();

protected:
  /// Return true if the cursor position is in the history area
  /// false if it is after the InteractivePosition.
  bool isCursorInHistoryArea()const;

  /// Reimplemented to make sure there is no text added into the
  /// history logs.
  virtual void insertFromMimeData(const QMimeData* source);

  /// Paste text at the current text cursor position.
  void pasteText(const QString& text);
public:

  /// A custom completer
  QPointer<ctkConsoleCompleter> Completer;

  /// Stores the beginning of the area of interactive input, outside which
  /// changes can't be made to the text edit contents
  int InteractivePosition;

  /// Indicates if the last statement processes was incomplete.
  bool MultilineStatement;

  /// Stores command-history, plus the current command buffer
  QStringList CommandHistory;

  /// Stores the current position in the command-history
  int CommandPosition;

  /// Prompt  color
  QColor PromptColor;

  /// Output text color
  QColor OutputTextColor;

  /// Error text color
  QColor ErrorTextColor;

  /// Standard input text color.
  QColor StdinTextColor;

  /// Command text color
  QColor CommandTextColor;

  /// Welcome text color
  QColor WelcomeTextColor;

  /// Primary prompt
  QString Ps1;

  /// Secondary prompt
  QString Ps2;

  ctkConsole::EditorHints EditorHints;

  bool ScrollbarAtBottom;

  QPointer<QEventLoop> InputEventLoop;
};


#endif

