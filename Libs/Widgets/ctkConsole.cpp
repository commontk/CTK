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
#include <QApplication>
#include <QClipboard>
#include <QCompleter>
#include <QKeyEvent>
#include <QPointer>
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
  MultilineStatement(false), Ps1("$ "), Ps2("> "),
  EditorHints(ctkConsole::AutomaticIndentation | ctkConsole::RemoveTrailingSpaces),
  ScrollbarAtBottom(false)
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

  QVBoxLayout * layout = new QVBoxLayout(q);
  layout->setMargin(0);
  layout->addWidget(this);

  connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)),
          SLOT(onScrollBarValueChanged(int)));
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

    QTextCursor text_cursor = this->textCursor();

    // Set to true if there's a current selection
    const bool selection = text_cursor.anchor() != text_cursor.position();
    // Set to true if the cursor overlaps the history area
    const bool history_area = this->isCursorInHistoryArea();

    // Allow copying anywhere in the console ...
    if(e->key() == Qt::Key_C && e->modifiers() == Qt::ControlModifier)
      {
      if(selection)
        {
        this->copy();
        }

      e->accept();
      return;
      }

    // Allow cut only if the selection is limited to the interactive area ...
    if(e->key() == Qt::Key_X && e->modifiers() == Qt::ControlModifier)
      {
      if(selection && !history_area)
        {
        this->cut();
        }

      e->accept();
      return;
      }

    // Force the cursor back to the interactive area
    if(history_area && e->key() != Qt::Key_Control)
      {
      text_cursor.setPosition(this->documentEnd());
      this->setTextCursor(text_cursor);
      }

    switch(e->key())
      {
      case Qt::Key_Up:
        e->accept();

        if (this->CommandPosition > 0)
          {
          this->replaceCommandBuffer(this->CommandHistory[--this->CommandPosition]);
          }
        break;

      case Qt::Key_Down:
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
        break;

      case Qt::Key_Left:
        if (text_cursor.position() > this->InteractivePosition)
          {
          this->Superclass::keyPressEvent(e);
          }
        else
          {
          e->accept();
          }
        break;

      case Qt::Key_Delete:
        e->accept();
        this->Superclass::keyPressEvent(e);
        this->updateCommandBuffer();
        break;

      case Qt::Key_Backspace:
        e->accept();
        if(text_cursor.position() > this->InteractivePosition)
          {
          this->Superclass::keyPressEvent(e);
          this->updateCommandBuffer();
          this->updateCompleterIfVisible();
          }
        break;

      case Qt::Key_Tab:
        e->accept();
        this->updateCompleter();
        this->selectCompletion();
        break;

      case Qt::Key_Home:
        e->accept();
        text_cursor.setPosition(this->InteractivePosition);
        this->setTextCursor(text_cursor);
        break;

      case Qt::Key_Return:
      case Qt::Key_Enter:
        e->accept();

        text_cursor.setPosition(this->documentEnd());
        this->setTextCursor(text_cursor);

        if (this->InputEventLoop.isNull())
          {
          this->internalExecuteCommand();
          }
        else
          {
          this->processInput();
          }
        break;

      default:
        e->accept();
        this->switchToUserInputTextColor();

        this->Superclass::keyPressEvent(e);
        this->updateCommandBuffer();
        this->updateCompleterIfVisible();
        break;
      }
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
  currentFormat.setForeground(color);
  this->setCurrentCharFormat(currentFormat);

  if (textCursorToUpdate)
    {
    QTextCharFormat textCursorFormat = textCursorToUpdate->charFormat();
    textCursorFormat.setForeground(color);
    textCursorToUpdate->setCharFormat(textCursorFormat);
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
    QTextCursor text_cursor = this->textCursor();
    text_cursor.setPosition(this->InteractivePosition, QTextCursor::KeepAnchor);
    QString commandText = text_cursor.selectedText();

    // Call the completer to update the completion model
    this->Completer->updateCompletionModel(commandText);

    // Place and show the completer if there are available completions
    if (this->Completer->completionCount())
      {
      // Get a QRect for the cursor at the start of the
      // current word and then translate it down 8 pixels.
      text_cursor = this->textCursor();
      text_cursor.movePosition(QTextCursor::StartOfWord);
      QRect cr = this->cursorRect(text_cursor);
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
void ctkConsolePrivate::updateCommandBuffer()
{
  this->commandBuffer() = this->toPlainText().mid(this->InteractivePosition);
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::replaceCommandBuffer(const QString& text)
{
  this->commandBuffer() = text;

  QTextCursor c(this->document());
  c.setPosition(this->InteractivePosition);
  c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  c.removeSelectedText();
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
  c.insertText("\n");

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
  this->promptForInput(indent);
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

  QTextCursor c(this->document());
  c.movePosition(QTextCursor::End);
  c.insertText("\n");

  this->InteractivePosition = this->documentEnd();

  this->InputEventLoop->exit();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::printString(const QString& text)
{
  this->textCursor().movePosition(QTextCursor::End);
  this->textCursor().insertText(text);
  this->InteractivePosition = this->documentEnd();
  this->ensureCursorVisible();
  this->scrollToBottom();
}

//----------------------------------------------------------------------------
void ctkConsolePrivate::printOutputMessage(const QString& text)
{
  Q_Q(ctkConsole);

  q->printMessage(text, q->outputTextColor());
}

//----------------------------------------------------------------------------
void ctkConsolePrivate::printErrorMessage(const QString& text)
{
  Q_Q(ctkConsole);

  q->printMessage(text, q->errorTextColor());
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
  QTextCursor text_cursor = this->textCursor();

  // If the cursor is currently on a clean line, do nothing, otherwise we move
  // the cursor to a new line before showing the prompt.
  text_cursor.movePosition(QTextCursor::StartOfLine);
  int startpos = text_cursor.position();
  text_cursor.movePosition(QTextCursor::EndOfLine);
  int endpos = text_cursor.position();
  if (endpos != startpos)
    {
    this->textCursor().insertText("\n");
    }

  this->textCursor().insertText(text);
  this->InteractivePosition = this->documentEnd();
  this->ensureCursorVisible();
  this->scrollToBottom();
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
  QTextCursor tc = this->textCursor();
  tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
  if (tc.selectedText()==".")
    {
    tc.insertText(QString(".") + completion);
    }
  else
    {
    tc = this->textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    this->setTextCursor(tc);
    }
  this->updateCommandBuffer();
}

//-----------------------------------------------------------------------------
void ctkConsolePrivate::onScrollBarValueChanged(int value)
{
  this->ScrollbarAtBottom = (this->verticalScrollBar()->maximum() == value);
}

//-----------------------------------------------------------------------------
bool ctkConsolePrivate::isCursorInHistoryArea()const
{
  return this->textCursor().anchor() < this->InteractivePosition
    || this->textCursor().position() < this->InteractivePosition;
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
  if(text.isNull())
    {
    return;
    }
  QTextCursor textCursor = this->textCursor();
  if (this->EditorHints & ctkConsole::SplitCopiedTextByLine)
    {
    QStringList lines = text.split(QRegExp("(?:\r\n|\r|\n)"));
    for(int i=0; i < lines.count(); ++i)
      {
      this->switchToUserInputTextColor(&textCursor);
      textCursor.insertText(lines.at(i));
      this->updateCommandBuffer();
      if (i < lines.count() - 1)
        {
        this->internalExecuteCommand();
        }
      }
    }
  else
    {
    this->switchToUserInputTextColor(&textCursor);
    textCursor.insertText(text);
    this->updateCommandBuffer();
    }
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
  QTextCursor tc = d->textCursor();
  tc.clearSelection();
  tc.setPosition(savedPosition);
  d->setTextCursor(tc);
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
void ctkConsole::exec(const QString& command)
{
  Q_D(ctkConsole);
  d->replaceCommandBuffer(command);
  d->internalExecuteCommand();
}

//-----------------------------------------------------------------------------
void ctkConsole::executeCommand(const QString& command)
{
  qWarning() << "ctkConsole::executeCommand not implemented !";
  qWarning() << "command:" << command;
}

//----------------------------------------------------------------------------
void ctkConsole::printMessage(const QString& message, const QColor& color)
{
  Q_D(ctkConsole);

  QTextCharFormat format = this->getFormat();
  format.setForeground(color);
  this->setFormat(format);
  d->printString(message);
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

  QScopedPointer<InputEventLoop> eventLoop(new InputEventLoop(qApp));
  d->InputEventLoop = QPointer<QEventLoop>(eventLoop.data());

  eventLoop->exec();

  return d->commandBuffer();
}

