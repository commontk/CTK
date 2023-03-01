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
/*=========================================================================

   Program: ParaView

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See http://www.paraview.org/paraview/project/license.html for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// Qt includes
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCompleter>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMimeData>
#include <QPointer>
#include <QPushButton>
#include <QTextBlock>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>

// CTK includes
#include "ctkConsole.h"
#include "ctkConsole_p.h"
#include "ctkPimpl.h"

//-----------------------------------------------------------------------------
// ctkConsoleCompleter methods

//-----------------------------------------------------------------------------
QStringList ctkConsoleCompleter::autocompletePreferenceList()
{
  return this->AutocompletePreferenceList;
}

//-----------------------------------------------------------------------------
void ctkConsoleCompleter::setAutocompletePreferenceList(const QStringList& list)
{
  this->AutocompletePreferenceList = list;
}

//-----------------------------------------------------------------------------
// ctkConsolePrivate methods

//-----------------------------------------------------------------------------
ctkConsolePrivate::ctkConsolePrivate(ctkConsole& object) :
  Superclass(0),
  q_ptr(&object),
  InteractivePosition(documentEnd()),
  MessageOutputSize(0),
  MultilineStatement(false), Ps1("$ "), Ps2("> "),
  insertCompletionMethod(true),
  EditorHints(ctkConsole::AutomaticIndentation | ctkConsole::RemoveTrailingSpaces),
  ScrollbarAtBottom(false),
  CompleterShortcuts(QList<QKeySequence>() << Qt::Key_Tab),
  RunFileOptions(ctkConsole::RunFileShortcut),
  RunFileButton(NULL),
  RunFileAction(NULL)
{
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::init()
{
  Q_Q(ctkConsole);
  this->setParent(q);
  this->setTabChangesFocus(false);
  this->setAcceptDrops(false);
  this->setAcceptRichText(false);
  this->setUndoRedoEnabled(false);

  this->PromptColor = QColor(0, 0, 0);    // Black
  this->OutputTextColor = QColor(0, 150, 0);  // Green
  this->MessageOutputColor = QColor(Qt::gray);  // Gray
  this->ErrorTextColor = QColor(255, 0, 0);   // Red
  this->StdinTextColor = QColor(Qt::darkGray);
  this->CommandTextColor = QColor(0, 0, 150); // Blue
  this->WelcomeTextColor = QColor(0, 0, 255); // Dark Blue

  QFont shellFont;
  shellFont.setFamily("Courier");
  shellFont.setStyleHint(QFont::TypeWriter);
  shellFont.setFixedPitch(true);

  QTextCharFormat format;
  format.setFont(shellFont);
  format.setForeground(this->OutputTextColor);
  this->setCurrentCharFormat(format);

  this->CommandHistory.append("");
  this->CommandPosition = 0;

  this->RunFileAction = new QAction(ctkConsole::tr("&Run file"), q);
  this->RunFileAction->setShortcut(ctkConsole::tr("Ctrl+g"));
  connect(this->RunFileAction, SIGNAL(triggered()), q, SLOT(runFile()));
  q->addAction(this->RunFileAction);

  QAction* printHelpAction = new QAction(ctkConsole::tr("Print &help"),q);
  printHelpAction->setShortcut(ctkConsole::tr("Ctrl+h"));
  connect(printHelpAction, SIGNAL(triggered()), q, SLOT(printHelp()));
  q->addAction(printHelpAction);

  this->RunFileButton = new QPushButton(q);
  this->RunFileButton->setText(ctkConsole::tr("&Run script from file"));
  this->RunFileButton->setVisible(false);

  QVBoxLayout * layout = new QVBoxLayout(q);
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(this);
  layout->addWidget(this->RunFileButton);

  connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)),
          SLOT(onScrollBarValueChanged(int)));
  connect(this, SIGNAL(textChanged()), SLOT(onTextChanged()));
  connect(this->RunFileButton, SIGNAL(clicked()), q, SLOT(runFile()));
  connect(this, SIGNAL(cursorPositionChanged()),
          q, SIGNAL(cursorPositionChanged()));
}

//-----------------------------------------------------------------------------
bool ctkConsolePrivate::isMoveLeftWithinLine(QKeyEvent* e, QTextCursor::MoveOperation &moveOperation, QTextCursor::MoveMode &moveMode)
{
  if (e == QKeySequence::MoveToPreviousChar)
    {
    moveOperation = QTextCursor::Left;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectPreviousChar)
    {
    moveOperation = QTextCursor::Left;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToPreviousWord)
    {
    moveOperation = QTextCursor::WordLeft;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectPreviousWord)
    {
    moveOperation = QTextCursor::WordLeft;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToStartOfLine)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectStartOfLine)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToStartOfBlock)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectStartOfBlock)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToStartOfDocument)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectStartOfDocument)
    {
    moveOperation = QTextCursor::StartOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else
    {
    return false;
    }
}

//-----------------------------------------------------------------------------
bool ctkConsolePrivate::isMoveRighttWithinLine(QKeyEvent* e, QTextCursor::MoveOperation &moveOperation, QTextCursor::MoveMode &moveMode)
{
  if (e == QKeySequence::MoveToNextChar)
    {
    moveOperation = QTextCursor::Right;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectNextChar)
    {
    moveOperation = QTextCursor::Right;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToNextWord)
    {
    moveOperation = QTextCursor::WordRight;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectNextWord)
    {
    moveOperation = QTextCursor::WordRight;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToEndOfLine)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectEndOfLine)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToEndOfBlock)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectEndOfBlock)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else if (e == QKeySequence::MoveToEndOfDocument)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::MoveAnchor;
    return true;
    }
  else if (e == QKeySequence::SelectEndOfDocument)
    {
    moveOperation = QTextCursor::EndOfLine;
    moveMode = QTextCursor::KeepAnchor;
    return true;
    }
  else
    {
    return false;
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::keyPressEvent(QKeyEvent* e)
{
  if (this->Completer && this->Completer->popup()->isVisible())
    {
    // The following keys are forwarded by the completer to the widget
    switch (e->key())
      {
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:
        e->ignore();
        return; // let the completer do default behavior
      default:
        break;
      }
    }

  QTextCursor textCursor = this->textCursor();

  // Set to true if there's a current selection
  const bool selection = (textCursor.anchor() != textCursor.position());
  // Set to true if the cursor overlaps the history area
  bool history_area = this->isCursorInHistoryArea();
  // The message output area is defined just under the command line
  // and it can display all messages catch during we autocomplete, etc.
  // Set to true if the cursor overlaps the message output area
  bool message_output_area = this->isCursorInMessageOutputArea();

  if (e->key() == Qt::Key_Escape)
    {
    if (selection)
      {
      // if a selection is active then Esc removes it
      textCursor.setPosition(textCursor.position());
      this->setTextCursor(textCursor);
      }
    else if (history_area || message_output_area)
      {
      // if not in the interactive area then Esc moves back to the end of the interactive area
      textCursor.setPosition(this->commandEnd());
      this->setTextCursor(textCursor);
      }
    else
      {
      // if in the interactive area and nothing is selected then Esc clears the current command
      this->replaceCommandBuffer("");
      // reset command history position (up-arrow will bring back the last command)
      if (this->CommandHistory.size() > 0)
        {
        this->CommandPosition = this->CommandHistory.size() - 1;
        }
      }
    e->accept();
    return;
    }

  // Allow copying anywhere in the console ...
  if(e == QKeySequence::Copy)
    {
    if(selection)
      {
      this->copy();
      }
    e->accept();
    return;
    }

  // Allow cut only if the selection is limited to the interactive area ...
  if(e == QKeySequence::Cut)
    {
    if(selection && !history_area && !message_output_area)
      {
      this->cut();
      this->updateCommandBuffer();
      }
    e->accept();
    return;
    }

  // Paste to the end of commandLine if in the history area or in message output area
  if(e == QKeySequence::Paste)
    {
    if(history_area || message_output_area)
      {
      textCursor.setPosition(this->commandEnd());
      this->setTextCursor(textCursor);
      }
    this->paste();
    this->updateCommandBuffer();
    e->accept();
    return;
    }

  // Allow vertical scrolling using page up/down
  if (e == QKeySequence::MoveToPreviousPage || e == QKeySequence::SelectPreviousPage
    || e == QKeySequence::MoveToNextPage || e == QKeySequence::SelectNextPage)
    {
    if (e == QKeySequence::SelectPreviousPage || e == QKeySequence::SelectNextPage)
      {
      // ignore
      e->accept();
      }
    else
      {
      this->Superclass::keyPressEvent(e);
      this->updateCommandBuffer();
      }
    return;
    }

  // Force the cursor back to the interactive area if anything else than copy/paste or page up/down is done
  // but only when a "real" key is pressed, not just a modifier (otherwise we could not press Control-c in the
  // history area because the cursor would jump to the interactive area immediately when Control is pressed)
  // Update: message_output_area is like the history_area: we can't modify it
  if( (history_area
       || message_output_area)
       && e->key() != Qt::Key_Control
       && e->key() != Qt::Key_Meta
       && e->key() != Qt::Key_Alt
       && e->key() != Qt::Key_Shift)
    {
    textCursor.setPosition(this->commandEnd());
    this->setTextCursor(textCursor);
    message_output_area = false;
    history_area = false;
    }

  // Start of line should be the start of interactive area
  QTextCursor::MoveOperation moveOperation = QTextCursor::NoMove;
  QTextCursor::MoveMode moveMode = QTextCursor::MoveAnchor;
  if(isMoveLeftWithinLine(e, moveOperation, moveMode))
    {
    textCursor.movePosition(moveOperation, moveMode);
    if (textCursor.position() >= this->InteractivePosition)
      {
      this->Superclass::keyPressEvent(e);
      }
    else
      {
      textCursor.setPosition(this->InteractivePosition, moveMode);
      this->setTextCursor(textCursor);
      e->accept();
      }
    this->updateCompleterIfVisible();
    return;
    }

  // End of line should be the end of interactive area
  moveOperation = QTextCursor::NoMove;
  moveMode = QTextCursor::MoveAnchor;
  if(isMoveRighttWithinLine(e, moveOperation, moveMode))
    {
    textCursor.movePosition(moveOperation, moveMode);
    if (textCursor.position() <= this->commandEnd())
      {
      this->Superclass::keyPressEvent(e);
      }
    else
      {
      textCursor.setPosition(this->commandEnd(), moveMode);
      this->setTextCursor(textCursor);
      e->accept();
      }
    this->updateCompleterIfVisible();
    return;
    }

  if (e == QKeySequence::Delete)
    {
    e->accept();
    // Can delete only if we are not at the end of the command line.
    // There is an exception if something (in the interactive area only) is selected,
    // because it will erase the text selected instead.
    if (textCursor.position() < this->commandEnd()
        || (textCursor.position() <= this->commandEnd()
             && selection && !message_output_area && !history_area))
      {
      this->Superclass::keyPressEvent(e);
      this->updateCommandBuffer();
      }
    return;
    }

  // There seems to be no QKeySequence for backspace, therefore the key
  // has to be recognized directly (the same way as it is done in the parent class)
  if (e->key() == Qt::Key_Backspace && !(e->modifiers() & ~Qt::ShiftModifier))
    {
    e->accept();
    // Can delete with backspace only if the cursor is after the InteractivePosition.
    // There is an exception if something (in the interactive area only) is selected,
    // because it will erase the text selected instead.
    if (textCursor.position() > this->InteractivePosition
        || (textCursor.position() >= this->InteractivePosition
             && selection  && !message_output_area && !history_area))
      {
      this->Superclass::keyPressEvent(e);
      this->updateCommandBuffer();
      this->updateCompleterIfVisible();
      }
    return;
    }

  if (e == QKeySequence::DeleteStartOfWord)
      {
      e->accept();
      if (textCursor.position() > this->InteractivePosition)
        {
        this->Superclass::keyPressEvent(e);
        this->updateCommandBuffer();
        this->updateCompleterIfVisible();
        }
      return;
      }

  if (e == QKeySequence::MoveToPreviousLine || e == QKeySequence::SelectPreviousLine)
    {
    e->accept();
    if (this->CommandPosition > 0)
      {
      this->replaceCommandBuffer(this->CommandHistory[--this->CommandPosition]);
      }
    return;
    }

  if (e == QKeySequence::MoveToNextLine || e == QKeySequence::SelectNextLine)
    {
    e->accept();
    if (this->CommandPosition < this->CommandHistory.size() - 2)
      {
      this->replaceCommandBuffer(this->CommandHistory[++this->CommandPosition]);
      }
    else
      {
      this->CommandPosition = this->CommandHistory.size()-1;
      this->replaceCommandBuffer("");
      }
    return;
    }

  if (e == QKeySequence::InsertParagraphSeparator)
    {
    e->accept();
    textCursor.setPosition(this->documentEnd());
    this->setTextCursor(textCursor);
    if (this->InputEventLoop.isNull())
      {
      this->internalExecuteCommand();
      }
    else
      {
      this->processInput();
      }
    return;
    }

  if (this->CompleterShortcuts.contains(e->key() + e->modifiers()))
    {
    e->accept();
    this->updateCompleter();
    this->selectCompletion();
    return;
    }

  e->accept();
  //Don't change the color of text outside the interactive area
  if (!message_output_area && !history_area)
    {
    this->switchToUserInputTextColor();
    }
  this->Superclass::keyPressEvent(e);
  this->updateCommandBuffer();
  this->updateCompleterIfVisible();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::switchToUserInputTextColor(QTextCursor* textCursorToUpdate)
{
  QColor color = this->CommandTextColor;
  if (!this->InputEventLoop.isNull())
    {
    color = this->StdinTextColor;
    }
  QTextCharFormat currentFormat = this->currentCharFormat();
  // Do not trigger a finishEdit for no reason. onTextChanged() would be called.
  if (currentFormat.foreground() != color)
    {
    currentFormat.setForeground(color);
    this->setCurrentCharFormat(currentFormat);
    }

  if (textCursorToUpdate)
    {
    QTextCharFormat textCursorFormat = textCursorToUpdate->charFormat();
    if (textCursorFormat.foreground() != color)
      {
      textCursorFormat.setForeground(color);
      textCursorToUpdate->setCharFormat(textCursorFormat);
      }
    }
}

//-----------------------------------------------------------------------------
int ctkConsolePrivate::documentEnd() const
{
  QTextCursor c(this->document());
  c.movePosition(QTextCursor::End);
  return c.position();
}

//-----------------------------------------------------------------------------
int ctkConsolePrivate::commandEnd() const
{
  QTextCursor c(this->document());
  c.setPosition(this->documentEnd()-this->MessageOutputSize);
  return c.position();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::focusOutEvent(QFocusEvent *e)
{
  this->Superclass::focusOutEvent(e);

  // For some reason the QCompleter tries to set the focus policy to
  // NoFocus, set let's make sure we set it back to the default WheelFocus.
  this->setFocusPolicy(Qt::WheelFocus);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::resizeEvent(QResizeEvent * e)
{
  this->Superclass::resizeEvent(e);

  if (this->ScrollbarAtBottom)
    {
    this->moveCursor(QTextCursor::End);
    this->scrollToBottom();
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::scrollToBottom()
{
  this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::updateCompleterIfVisible()
{
  if (this->Completer && this->Completer->popup()->isVisible())
    {
    this->updateCompleter();
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::selectCompletion()
{
  if (this->Completer && this->Completer->completionCount() == 1)
    {
    this->insertCompletion(this->Completer->currentCompletion());
    this->Completer->popup()->hide();
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::setCompleter(ctkConsoleCompleter* completer)
{
  if (this->Completer)
    {
    this->Completer->setWidget(0);
    disconnect(this->Completer, SIGNAL(activated(QString)),
               this, SLOT(insertCompletion(QString)));

    }
  this->Completer = completer;
  if (this->Completer)
    {
    this->Completer->setWidget(this);
    connect(this->Completer, SIGNAL(activated(QString)),
            this, SLOT(insertCompletion(QString)));
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::updateCompleter()
{
  if (this->Completer)
    {
    // Get the text between the current cursor position
    // and the start of the line
    QTextCursor textCursor = this->textCursor();
    while (!textCursor.selectedText().contains(q_ptr->ps1()))
      {
      textCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
      }

    // search through the text the multiline statement symbol "... " + 1 char (go to the line char)
    QString commandText = textCursor.selectedText();
    int pos_Ps2 = commandText.indexOf(q_ptr->ps2())-1;
    while (pos_Ps2 > -1)
      {
      // remove the multiline symbol + the previous character due to "enter"
      int number_deleted_char=q_ptr->ps2().size()+1;
      // remove the line continuation character '\' if it's here
      if (commandText.at(pos_Ps2-1) == QChar('\\') )
        {
        pos_Ps2--;
        number_deleted_char++;
        }
      commandText.remove(pos_Ps2,number_deleted_char);
      pos_Ps2 = commandText.indexOf(q_ptr->ps2())-1;
      }
    commandText.remove(q_ptr->ps1());
    commandText.remove('\r'); // not recongnize by python
    //commandText.replace(QRegExp("\\s*$"), ""); // Remove trailing spaces ---- DOESN'T WORK ----

    // Save current positions: Since some implementation of
    // updateCompletionModel (e.g python) can display messages
    // while building the completion model, it is important to save
    // and restore the positions.
    int savedInteractivePosition = this->InteractivePosition;
    int savedCursorPosition = this->textCursor().position();

    //move the cursor at the end in case of a message displayed
    textCursor = this->textCursor();
    textCursor.setPosition(this->documentEnd());
    this->setTextCursor(textCursor);
    // Save color of displayed message
    QColor savedOutputTextColor = this->OutputTextColor;
    QColor savedErrorTextColor = this->ErrorTextColor;
    // Change color of displayed message in message_output_area
    this->OutputTextColor = this->MessageOutputColor;
    this->ErrorTextColor = this->MessageOutputColor;

    // Call the completer to update the completion model
    this->Completer->updateCompletionModel(commandText);

    // Restore Color
    this->OutputTextColor = savedOutputTextColor;
    this->ErrorTextColor = savedErrorTextColor;

    // Restore positions
    this->InteractivePosition = savedInteractivePosition;
    textCursor = this->textCursor();
    textCursor.setPosition(savedCursorPosition);
    this->setTextCursor(textCursor);


    // Place and show the completer if there are available completions
    if (this->Completer->completionCount())
      {
      // Get a QRect for the cursor at the start of the
      // current word and then translate it down 8 pixels.
      textCursor = this->textCursor();
      textCursor.movePosition(QTextCursor::StartOfWord);
      QRect cr = this->cursorRect(textCursor);
      cr.translate(0,8);
      cr.setWidth(this->Completer->popup()->sizeHintForColumn(0)
        + this->Completer->popup()->verticalScrollBar()->sizeHint().width());
      this->Completer->complete(cr);
      }
    else
      {
      this->Completer->popup()->hide();
      }
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::updateCommandBuffer(int commandLength)
{
  if (commandLength == -1)
    {
    commandLength =
        this->commandEnd() - this->InteractivePosition;
    }
  this->commandBuffer() =
      this->toPlainText().mid(this->InteractivePosition, commandLength);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::replaceCommandBuffer(const QString& text)
{
  this->commandBuffer() = text;

  QTextCursor c(this->document());
  c.setPosition(this->InteractivePosition);
  c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  c.removeSelectedText();
  // all text removed, we need then to re-init our tracker on the message output area
  this->MessageOutputSize = 0;
  this->switchToUserInputTextColor(&c);
  c.insertText(text);
}

//-----------------------------------------------------------------------------
QString& ctkConsolePrivate::commandBuffer()
{
  return this->CommandHistory.back();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::internalExecuteCommand()
{
  Q_Q(ctkConsole);

  QString command = this->commandBuffer();
  if (this->EditorHints & ctkConsole::RemoveTrailingSpaces)
    {
    command.replace(QRegExp("\\s*$"), ""); // Remove trailing spaces
    this->commandBuffer() = command; // Update buffer
    }

  // First update the history cache. It's essential to update the
  // this->CommandPosition before calling internalExecuteCommand() since that
  // can result in a clearing of the current command (BUG #8765).
  if (!command.isEmpty()) // Don't store empty commands in the history
    {
    this->CommandHistory.push_back("");
    this->CommandPosition = this->CommandHistory.size() - 1;
    }

  QTextCursor c(this->document());
  c.movePosition(QTextCursor::End);

  this->InteractivePosition = this->documentEnd();

  emit q->aboutToExecute(command);
  q->executeCommand(command);
  emit q->executed(command);

  // Find the indent for the command.
  QString indent;
  if (this->EditorHints & ctkConsole::AutomaticIndentation)
    {
    QRegExp regExp("^(\\s+)");
    if (regExp.indexIn(command) != -1)
      {
      indent = regExp.cap(1);
      }
    }

  // Give a chance for log messages to appear before displaying the new prompt.
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

  this->promptForInput(indent);
  this->MessageOutputSize = 0;
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::processInput()
{
  QString command = this->commandBuffer();

  if (this->EditorHints & ctkConsole::RemoveTrailingSpaces)
    {
    command.replace(QRegExp("\\s*$"), ""); // Remove trailing spaces
    this->commandBuffer() = command; // Update buffer
    }

  QTextCursor textCursor = this->textCursor();
  textCursor.movePosition(QTextCursor::End);
  textCursor.insertText("\n");

  this->InteractivePosition = this->documentEnd();

  this->InputEventLoop->exit();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::printString(const QString& text)
{
  QTextCursor textCursor = this->textCursor();
  textCursor.movePosition(QTextCursor::End);
  textCursor.insertText(text);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::printCommand(const QString& cmd)
{
  this->textCursor().insertText(cmd);
  this->updateCommandBuffer();
}

//----------------------------------------------------------------------------
void ctkConsolePrivate::promptForInput(const QString& indent)
{
  Q_Q(ctkConsole);

  QTextCharFormat format = q->getFormat();
  format.setForeground(q->promptColor());
  q->setFormat(format);

  if(!this->MultilineStatement)
    {
    this->prompt(q->ps1());
    this->MessageOutputSize=0;
    }
  else
    {
    this->prompt(q->ps2());
    }
  this->printCommand(indent);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::prompt(const QString& text)
{
  QTextCursor textCursor = this->textCursor();

  // If the cursor is currently on a clean line, do nothing, otherwise we move
  // the cursor to a new line before showing the prompt.
  textCursor.movePosition(QTextCursor::StartOfLine);
  int startpos = textCursor.position();
  textCursor.movePosition(QTextCursor::EndOfLine);
  int endpos = textCursor.position();
  if (endpos != startpos)
    {
    this->textCursor().insertText("\n");
    }

  this->textCursor().insertText(text);
  this->InteractivePosition = this->documentEnd();
}

//----------------------------------------------------------------------------
void ctkConsolePrivate::printWelcomeMessage()
{
  Q_Q(ctkConsole);

  q->printMessage(
    QLatin1String("CTK Console"),
    q->welcomeTextColor());
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::insertCompletion(const QString& completion)
{
  Q_Q(ctkConsole);
  QTextCursor textCursor = this->textCursor();
  textCursor.setPosition(this->commandEnd());
  // save the initial cursor position
  QTextCursor endOfCompletion = this->textCursor();
  endOfCompletion.setPosition(this->commandEnd());
  // Select the previous character
  textCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
  QString charBeforeCompletion = textCursor.selectedText();
  if (charBeforeCompletion == ".")
    {
    textCursor.insertText(QString(".") + completion);
    }
  else
    {
    //can't more autocomplete when cursor right after '(' or ')'
    if (charBeforeCompletion == ")" || charBeforeCompletion == "(")
      {
      return;
      }
    textCursor.clearSelection();
    textCursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    if (insertCompletionMethod)
      {
      textCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
      }
    else
      {
      textCursor.setPosition(endOfCompletion.position(), QTextCursor::KeepAnchor);
      }
    textCursor.insertText(completion);
    endOfCompletion.setPosition(textCursor.position());
    this->setTextCursor(textCursor);
    }

  // Get back the whole command line to apply a cursor offset
  // (moving cursor between parenthsesis if the completion is
  // a callable object with more than the self argument)
  // StartOfBlock don't catch the whole command line if multi-line statement
  textCursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::KeepAnchor);
  QString shellLine = textCursor.selectedText();
  shellLine.replace(q->ps1(), "");
  shellLine.replace(q->ps2(), "");
  textCursor.setPosition(endOfCompletion.position());
  this->setTextCursor(textCursor);
  int cursorOffset = this->Completer->cursorOffset(shellLine);
  textCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, cursorOffset);
  this->setTextCursor(textCursor);
  this->updateCommandBuffer();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::onScrollBarValueChanged(int value)
{
  this->ScrollbarAtBottom = (this->verticalScrollBar()->maximum() == value);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::onTextChanged()
{
  this->scrollToBottom();
  this->ensureCursorVisible();
}

//-----------------------------------------------------------------------------
bool ctkConsolePrivate::isCursorInHistoryArea()const
{
  return this->textCursor().anchor() < this->InteractivePosition
    || this->textCursor().position() < this->InteractivePosition;
}

//-----------------------------------------------------------------------------
bool ctkConsolePrivate::isCursorInMessageOutputArea()const
{
  return this->textCursor().anchor() > this->commandEnd()
    || this->textCursor().position() > this->commandEnd();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::insertFromMimeData(const QMimeData* source)
{
  if (this->isCursorInHistoryArea())
    {
    QTextCursor textCursor = this->textCursor();
    textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    this->setTextCursor(textCursor);
    }
  const QString text = source->text();
  if (!text.isEmpty())
    {
    this->pasteText(text);
    }
  else
    {
    this->Superclass::insertFromMimeData(source);
    }
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::pasteText(const QString& text)
{
  Q_Q(ctkConsole);

  if(text.isNull())
    {
    return;
    }

  // Execute multiline commands at once, for more robust command parsing and execution.
  //
  // For example, an empty line in a Python function could not be executed line-by-line, because
  // then empty line would be interpreted as the end of the function.
  // In contrast, the empty line would not cause any issue when execuing the whole string at once
  // (the empty line would be just ignored).
  //
  // We cannot execute at once if a command is already being edited (some command has been already typed in the current line)
  // or a muiltiline statement is being entered).
  if (!this->MultilineStatement  && this->commandBuffer().trimmed().isEmpty()
      && text.trimmed().contains("\n"))
    {
    // Execute at once
    q->printMessage("\n" + text.trimmed(), this->StdinTextColor);
    q->executeString(text);

    // Give a chance for log messages to appear before displaying the new prompt.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    this->promptForInput();
    return;
    }

  QTextCursor textCursor = this->textCursor();

  // if there is anything else after the cursor position
  // we have to remove it and paste it in the last line
  textCursor.setPosition(this->commandEnd(), QTextCursor::KeepAnchor);
  QString endOfCommand = textCursor.selectedText();
  textCursor.removeSelectedText();

  if (this->EditorHints & ctkConsole::SplitCopiedTextByLine)
    {
    // Execute line by line
    QStringList lines = text.split(QRegExp("(?:\r\n|\r|\n)"));
    for(int i=0; i < lines.count(); ++i)
      {
      this->switchToUserInputTextColor(&textCursor);
      textCursor.insertText(lines.at(i));
      this->updateCommandBuffer();
      // if it's not the last line to paste
      if (i < lines.count() - 1)
        {
        // be sure to go to the end of document
        // warn about wrong paste if there is something in the message_output_area
        textCursor.setPosition(this->documentEnd());
        this->setTextCursor(textCursor);

        this->internalExecuteCommand();
        }
      }
    }
  else
    {
    // Execute all the lines as one string, using the the line-by-line command execution method.
    this->switchToUserInputTextColor(&textCursor);
    textCursor.insertText(text);
    }

  // add back the rest of the line after the current position
  int position = textCursor.position();
  textCursor.insertText(endOfCommand);
  textCursor.setPosition(position);
  this->setTextCursor(textCursor);

  this->updateCommandBuffer();
}

//-----------------------------------------------------------------------------
// ctkConsole methods

//-----------------------------------------------------------------------------
ctkConsole::ctkConsole(QWidget* parentObject) :
  Superclass(parentObject),
  d_ptr(new ctkConsolePrivate(*this))
{
  Q_D(ctkConsole);
  d->init();
}

//-----------------------------------------------------------------------------
ctkConsole::ctkConsole(ctkConsolePrivate * pimpl, QWidget* parentObject) :
  QWidget(parentObject), d_ptr(pimpl)
{
  Q_D(ctkConsole);
  d->init();
}

//-----------------------------------------------------------------------------
ctkConsole::~ctkConsole()
{
}

//-----------------------------------------------------------------------------
QTextCharFormat ctkConsole::getFormat() const
{
  Q_D(const ctkConsole);
  return d->currentCharFormat();
}

//-----------------------------------------------------------------------------
void ctkConsole::setFormat(const QTextCharFormat& Format)
{
  Q_D(ctkConsole);
  d->setCurrentCharFormat(Format);
}

//-----------------------------------------------------------------------------
QFont ctkConsole::shellFont() const
{
  Q_D(const ctkConsole);
  return d->currentFont();
}

//-----------------------------------------------------------------------------
void ctkConsole::setShellFont(const QFont& font)
{
  Q_D(ctkConsole);
  int savedPosition = d->textCursor().position();
  d->selectAll();
  d->setCurrentFont(font);
  QTextCursor textCursor = d->textCursor();
  textCursor.clearSelection();
  textCursor.setPosition(savedPosition);
  d->setTextCursor(textCursor);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, ctkConsoleCompleter*, completer, Completer);

//-----------------------------------------------------------------------------
void ctkConsole::setCompleter(ctkConsoleCompleter* completer)
{
  Q_D(ctkConsole);
  d->setCompleter(completer);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, promptColor, PromptColor);
CTK_SET_CPP(ctkConsole, const QColor&, setPromptColor, PromptColor);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, outputTextColor, OutputTextColor);
CTK_SET_CPP(ctkConsole, const QColor&, setOutputTextColor, OutputTextColor);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, errorTextColor, ErrorTextColor);
CTK_SET_CPP(ctkConsole, const QColor&, setErrorTextColor, ErrorTextColor);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, stdinTextColor, StdinTextColor);
CTK_SET_CPP(ctkConsole, const QColor&, setStdinTextColor, StdinTextColor);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, commandTextColor, CommandTextColor);
CTK_SET_CPP(ctkConsole, const QColor&, setCommandTextColor, CommandTextColor);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QColor, welcomeTextColor, WelcomeTextColor);
CTK_SET_CPP(ctkConsole, const QColor&, setWelcomeTextColor, WelcomeTextColor);

//-----------------------------------------------------------------------------
QColor ctkConsole::backgroundColor()const
{
  Q_D(const ctkConsole);
  QPalette pal = d->palette();
  return pal.color(QPalette::Base);
}

//-----------------------------------------------------------------------------
void ctkConsole::setBackgroundColor(const QColor& newColor)
{
  Q_D(ctkConsole);
  if (this->backgroundColor() == newColor)
    {
    return;
    }
  QPalette pal = this->palette();
  pal.setColor(QPalette::Base, newColor);
  d->setPalette(pal);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QString, ps1, Ps1);
CTK_SET_CPP(ctkConsole, const QString&, setPs1, Ps1);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QString, ps2, Ps2);
CTK_SET_CPP(ctkConsole, const QString&, setPs2, Ps2);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, ctkConsole::EditorHints, editorHints, EditorHints);
CTK_SET_CPP(ctkConsole, const ctkConsole::EditorHints&, setEditorHints, EditorHints);

//-----------------------------------------------------------------------------
int ctkConsole::cursorPosition() const
{
  Q_D(const ctkConsole);
  return d->textCursor().position();
}

//-----------------------------------------------------------------------------
int ctkConsole::cursorColumn() const
{
  Q_D(const ctkConsole);
  QTextCursor cursor = d->textCursor();
  cursor.movePosition(QTextCursor::StartOfLine);
  return d->textCursor().position() - cursor.position();
}

//-----------------------------------------------------------------------------
int ctkConsole::cursorLine() const
{
  Q_D(const ctkConsole);
  QTextCursor textCursor = d->textCursor();
  textCursor.movePosition(QTextCursor::StartOfLine);
  int lines = 1;
  QTextBlock block = textCursor.block().previous();
  while(block.isValid())
    {
    lines += block.lineCount();
    block = block.previous();
    }
  return lines;
}

//-----------------------------------------------------------------------------
Qt::ScrollBarPolicy ctkConsole::scrollBarPolicy()const
{
  Q_D(const ctkConsole);
  return d->verticalScrollBarPolicy();
}

//-----------------------------------------------------------------------------
void ctkConsole::setScrollBarPolicy(const Qt::ScrollBarPolicy& newScrollBarPolicy)
{
  Q_D(ctkConsole);
  d->setVerticalScrollBarPolicy(newScrollBarPolicy);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, QList<QKeySequence>, completerShortcuts, CompleterShortcuts);

//-----------------------------------------------------------------------------
void ctkConsole::setCompleterShortcuts(const QList<QKeySequence>& keys)
{
  Q_D(ctkConsole);
  d->CompleterShortcuts = keys;
}

//-----------------------------------------------------------------------------
void ctkConsole::addCompleterShortcut(const QKeySequence& key)
{
  Q_D(ctkConsole);
  if (!d->CompleterShortcuts.contains(key))
    {
    d->CompleterShortcuts.append(key);
    }
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkConsole, ctkConsole::RunFileOptions, runFileOptions, RunFileOptions);

//-----------------------------------------------------------------------------
void ctkConsole::setRunFileOptions(const RunFileOptions& newOptions)
{
  Q_D(ctkConsole);
  d->RunFileButton->setVisible(newOptions.testFlag(ctkConsole::RunFileButton));
  d->RunFileAction->setEnabled(newOptions.testFlag(ctkConsole::RunFileShortcut));
}

//-----------------------------------------------------------------------------
const QString& ctkConsole::commandBuffer()
{
  Q_D(ctkConsole);
  return d->commandBuffer();
}

//-----------------------------------------------------------------------------
void ctkConsole::setCommandBuffer(const QString& command)
{
  Q_D(ctkConsole);
  d->replaceCommandBuffer(command);
}

//-----------------------------------------------------------------------------
const QStringList& ctkConsole::commandHistory()
{
  Q_D(ctkConsole);
  return d->CommandHistory;
}

//-----------------------------------------------------------------------------
void ctkConsole::setCommandHistory(const QStringList& commandHistory)
{
  Q_D(ctkConsole);
  d->CommandHistory = commandHistory;
  d->CommandHistory.append("");
  d->CommandPosition = d->CommandHistory.size()-1;
}

//-----------------------------------------------------------------------------
void ctkConsole::exec(const QString& command)
{
  Q_D(ctkConsole);
  d->replaceCommandBuffer(command);
  d->internalExecuteCommand();
}

//-----------------------------------------------------------------------------
void ctkConsole::runFile(const QString& filePath)
{
  Q_D(ctkConsole);
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly))
    {
    qWarning() << tr( "File '%1' can't be read.").arg(filePath);
    return;
    }

  // Save the current command, use it later
  QString savedCommand = d->commandBuffer();
  d->replaceCommandBuffer("");

  // Read the commands from the file
  QTextStream fileStream(&file);
  QString commands = fileStream.readAll();

  // Print the commands on the console and execute them
  this->printMessage("\n" + commands.trimmed() + "\n", d->StdinTextColor);
  this->executeString(commands);

  // Give a chance for log messages to appear before displaying the new prompt.
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

  // Display a new command prompt and allow the user to resume typing the command.
  d->promptForInput();
  d->replaceCommandBuffer(savedCommand);
}

//-----------------------------------------------------------------------------
void ctkConsole::runFile()
{
  Q_D(ctkConsole);
  QString filePath =
    QFileDialog::getOpenFileName(this, tr("Select a script file to run"), d->LastRunFile);
  if (!filePath.isEmpty())
    {
    d->LastRunFile = filePath;
    this->runFile(filePath);
    }
}

//-----------------------------------------------------------------------------
void ctkConsole::printHelp()
{
  this->printMessage("\n", Qt::gray);
  this->printMessage(tr("CTRL+h: Print this help message\n"), Qt::gray);
  this->printMessage(tr("CTRL+r: Open a file dialog to select a file to run\n"), Qt::gray);
}

//-----------------------------------------------------------------------------
void ctkConsole::executeCommand(const QString& command)
{
  qWarning() << "ctkConsole::executeCommand not implemented !";
  qWarning() << "command:" << command;
}

//-----------------------------------------------------------------------------
void ctkConsole::executeString(const QString& commands)
{
  qWarning() << "ctkConsole::executeString not implemented !";
  qWarning() << "commands:" << commands;
}

//----------------------------------------------------------------------------
void ctkConsole::printMessage(const QString& message, const QColor& color)
{
  Q_D(ctkConsole);

  // Jump to the end and print text with the selected color

  QTextCursor textCursor = d->textCursor();
  textCursor.movePosition(QTextCursor::End);

  QTextCharFormat format = this->getFormat();
  format.setForeground(color);

  textCursor.insertText(message, format);
}

//----------------------------------------------------------------------------
void ctkConsole::printOutputMessage(const QString& text)
{
  Q_D(ctkConsole);
  QString textToPrint = text;
  if (d->MessageOutputSize == 0)
    {
    textToPrint.prepend("\n");
    }
  d->MessageOutputSize += textToPrint.size();
  this->printMessage(textToPrint, this->outputTextColor());
}

//----------------------------------------------------------------------------
void ctkConsole::printErrorMessage(const QString& text)
{
  Q_D(ctkConsole);
  QString textToPrint = text;
  if (d->MessageOutputSize == 0)
    {
    textToPrint.prepend("\n");
    }
  d->MessageOutputSize += textToPrint.size();
  this->printMessage(textToPrint, this->errorTextColor());
}

//-----------------------------------------------------------------------------
void ctkConsole::clear()
{
  Q_D(ctkConsole);

  d->clear();

  // For some reason the QCompleter tries to set the focus policy to
  // NoFocus, set let's make sure we set it back to the default WheelFocus.
  d->setFocusPolicy(Qt::WheelFocus);

  d->promptForInput();
}

//-----------------------------------------------------------------------------
void ctkConsole::reset()
{
  Q_D(ctkConsole);

  d->clear();

  // For some reason the QCompleter tries to set the focus policy to
  // NoFocus, set let's make sure we set it back to the default WheelFocus.
  d->setFocusPolicy(Qt::WheelFocus);

  d->printWelcomeMessage();
  d->promptForInput();
}

//-----------------------------------------------------------------------------
QString ctkConsole::stdInRedirectCallBack(void * callData)
{
  ctkConsole * self = reinterpret_cast<ctkConsole*>(callData);
  Q_ASSERT(self);
  if (!self)
    {
    return QLatin1String("");
    }

  return self->readInputLine();
}

namespace
{
class InputEventLoop : public QEventLoop
{
public:
  InputEventLoop(QApplication * app, QObject * parentObject = 0) :
    QEventLoop(parentObject), App(app){}
  virtual bool processEvents(ProcessEventsFlags flags = AllEvents)
    {
    this->App->processEvents(flags);
    return true;
    }
  QApplication * App;
};

}

//-----------------------------------------------------------------------------
QString ctkConsole::readInputLine()
{
  Q_D(ctkConsole);

  d->moveCursor(QTextCursor::End);
  d->InteractivePosition = d->documentEnd();
  d->MessageOutputSize = 0;

  QScopedPointer<InputEventLoop> eventLoop(new InputEventLoop(qApp));
  d->InputEventLoop = QPointer<QEventLoop>(eventLoop.data());

  eventLoop->exec();

  return d->commandBuffer();
}

//-----------------------------------------------------------------------------
int ctkConsole::maxVisibleCompleterItems() const
{
  Q_D(const ctkConsole);
  if (!this->completer())
    {
    return 0;
    }
  return this->completer()->maxVisibleItems();
}

//-----------------------------------------------------------------------------
void ctkConsole::setMaxVisibleCompleterItems(int count)
{
  Q_D(ctkConsole);
  if (!this->completer())
    {
    qWarning() << Q_FUNC_INFO << " failed: invalid completer";
    return;
    }
  this->completer()->setMaxVisibleItems(count);
}
