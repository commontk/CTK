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

#ifndef __ctkConsole_h
#define __ctkConsole_h

// Qt includes
#include <QWidget>
#include <QTextCharFormat>
#include <QCompleter>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkConsolePrivate;
class ctkConsoleCompleter;

/// \ingroup Widgets
/// QWidget that provides an interactive console
class CTK_WIDGETS_EXPORT ctkConsole : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QColor promptColor READ promptColor WRITE setPromptColor)
  Q_PROPERTY(QColor outputTextColor READ outputTextColor WRITE setOutputTextColor)
  Q_PROPERTY(QColor errorTextColor READ errorTextColor WRITE setErrorTextColor)
  Q_PROPERTY(QColor stdinTextColor READ stdinTextColor WRITE setStdinTextColor)
  Q_PROPERTY(QColor commandTextColor READ commandTextColor WRITE setCommandTextColor)
  Q_PROPERTY(QColor welcomeTextColor READ welcomeTextColor WRITE setWelcomeTextColor)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QString ps1 READ ps1 WRITE setPs1)
  Q_PROPERTY(QString ps2 READ ps2 WRITE setPs2)
  Q_PROPERTY(int cursorPosition READ cursorPosition)
  Q_PROPERTY(int cursorColumn READ cursorColumn)
  Q_PROPERTY(int cursorLine READ cursorLine)
  Q_FLAGS(EditorHint EditorHints)
  Q_PROPERTY(EditorHints editorHints READ editorHints WRITE setEditorHints)
  Q_ENUMS(Qt::ScrollBarPolicy)
  Q_PROPERTY(Qt::ScrollBarPolicy scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy)
  Q_PROPERTY(QList<QKeySequence> completerShortcuts READ completerShortcuts WRITE setCompleterShortcuts)
  Q_FLAGS(RunFileOption RunFileOptions)
  Q_PROPERTY(RunFileOptions runFileOptions READ runFileOptions WRITE setRunFileOptions)
  Q_PROPERTY(int maxVisibleCompleterItems READ maxVisibleCompleterItems WRITE setMaxVisibleCompleterItems)
  Q_PROPERTY(QString commandBuffer READ commandBuffer WRITE setCommandBuffer)
  Q_PROPERTY(QStringList commandHistory READ commandHistory WRITE setCommandHistory)

public:

  enum EditorHint
  {
    NoHints = 0x00,
    AutomaticIndentation = 0x01, /*!< Align cursor based an indentation of the previous command */
    RemoveTrailingSpaces = 0x02, /*!< Remove trailing spaces of the entered command */
    SplitCopiedTextByLine = 0x4  /*!< Copied text is split by lines and each one is executed (expected the last one) */
  };
  Q_DECLARE_FLAGS(EditorHints, EditorHint)

  enum RunFileOption
  {
    NoRunFileUserInterface = 0x00,
    RunFileButton = 0x01,   /*!< Show a button at the bottom of the console to run a file */
    RunFileShortcut = 0x02, /*!< Add the shortcut CTRL+r to run a file */
  };
  Q_DECLARE_FLAGS(RunFileOptions, RunFileOption)

  ctkConsole(QWidget* parentObject = 0);
  typedef QWidget Superclass;
  virtual ~ctkConsole();

  /// Returns the current formatting that will be used by printMessage()
  Q_INVOKABLE QTextCharFormat getFormat() const;
  
  /// Sets formatting that will be used by printMessage()
  Q_INVOKABLE void setFormat(const QTextCharFormat& Format);

  /// Returns current font of python shell
  Q_INVOKABLE QFont shellFont() const;

  /// Sets font of python shell
  Q_INVOKABLE void setShellFont(const QFont& font);

  /// Return the completer of this console
  Q_INVOKABLE ctkConsoleCompleter* completer() const;

  /// Set a completer for this console
  Q_INVOKABLE void setCompleter(ctkConsoleCompleter* completer);

  QColor promptColor()const;

  /// \sa promptColor()
  void setPromptColor(const QColor& newColor);

  QColor outputTextColor()const;

  /// \sa outputTextColor()
  void setOutputTextColor(const QColor& newColor);

  QColor errorTextColor()const;

  /// \sa errorTextColor()
  void setErrorTextColor(const QColor& newColor);

  QColor stdinTextColor()const;

  /// \sa stdinTextColor()
  void setStdinTextColor(const QColor& newColor);

  QColor commandTextColor()const;

  /// \sa commandTextColor()
  void setCommandTextColor(const QColor& newColor);

  QColor welcomeTextColor()const;

  /// \sa welcomeTextColor()
  void setWelcomeTextColor(const QColor& newColor);

  QColor backgroundColor()const;

  void setBackgroundColor(const QColor& newColor);

  EditorHints editorHints()const;

  /// \sa editorHints()
  void setEditorHints(const EditorHints& newEditorHints);

  Qt::ScrollBarPolicy scrollBarPolicy()const;

  /// \sa scrollBarPolicy()
  void setScrollBarPolicy(const Qt::ScrollBarPolicy& newScrollBarPolicy);

  /// Returns the string used as primary prompt
  virtual QString ps1() const;

  /// Set the string used as primary prompt
  virtual void setPs1(const QString& newPs1);

  /// Returns the string used as secondary prompt
  virtual QString ps2() const;

  /// Set the string used as secondary prompt
  virtual void setPs2(const QString& newPs2);

  /// Returns the absolute position of the cursor within the console.
  /// \sa cursorPositionChanged(int)
  int cursorPosition() const;

  /// Returns the column of the cursor within the console.
  /// \sa cursorPositionChanged(int)
  int cursorColumn() const;

  /// Returns the line of the cursor within the console.
  /// \sa cursorPositionChanged(int)
  int cursorLine() const;

  /// Get maximum number of items shown in the auto-complete popup.
  int maxVisibleCompleterItems() const;

  /// Set maximum number of items shown in the auto-complete popup.
  void setMaxVisibleCompleterItems(int);

  static QString stdInRedirectCallBack(void * callData);

  /// Get the list of shortcuts that trigger the completion options.
  /// \sa setCompleterShortcuts(), addCompleterShortcut()
  QList<QKeySequence> completerShortcuts()const;

  /// Set the list of shortcuts showing the completion options.
  /// Default is simply Qt::Key_Tab.
  /// \sa completerShortcuts(), addCompleterShortcut()
  void setCompleterShortcuts(const QList<QKeySequence>& keys);

  /// Convenience method to add a key sequence to the list of shortcuts that
  /// show the completion options.
  /// \sa completerShortcuts(), setCompleterShortcuts()
  void addCompleterShortcut(const QKeySequence& key);

  RunFileOptions runFileOptions()const;

  /// Set which options to run file are enabled.
  /// Default is RunFileShortcut.
  /// \sa runFileOptions()
  void setRunFileOptions(const RunFileOptions& newOptions);

  /// Get the current command buffer (text on current input line, not yet executed)
  /// \sa setCommandBuffer()
  virtual const QString& commandBuffer();

  /// Get the command history list (previously executed commands)
  /// \sa commandBuffer()
  /// \sa setCommandBuffer()
  virtual const QStringList& commandHistory();

Q_SIGNALS:

  /// This signal emitted before and after a command is executed
  void aboutToExecute(const QString&);
  void executed(const QString&);

  /// This signal is emitted whenever the position of the cursor changed.
  /// \sa cursorPosition()
  void cursorPositionChanged();

public Q_SLOTS:

  /// Clears the contents of the console
  virtual void clear();

  /// Clears the contents of the console and display welcome message
  virtual void reset();

  /// Set the the command buffer (pending command for user)
  /// \sa commandBuffer()
  virtual void setCommandBuffer(const QString&);

  /// Set the command history (e.g. if restored from file)
  /// \sa commandHistory()
  virtual void setCommandHistory(const QStringList&);

  /// Exec the contents of the last console line
  /// \sa openFile(), runFile(QString)
  virtual void exec(const QString&);

  /// Exec line by line the content of a file.
  /// \sa openFile(), exec()
  virtual void runFile(const QString& filePath);

  /// Open a file dialog to select a file and run it.
  /// Shortcut: CTRL+O
  /// \sa runFile(QString), exec()
  virtual void runFile();

  /// Print the console help with shortcuts.
  virtual void printHelp();

  /// Prints text on the console
  void printMessage(const QString& message, const QColor& color);

  /// Print a message
  /// \sa ctkConsole::outputTextColor
  void printOutputMessage(const QString& text);

  /// Print a message
  /// \sa ctkConsole::errorTextColor
  void printErrorMessage(const QString& text);

protected:

  /// Prompt the user for input
  QString readInputLine();

  /// Called whenever the user enters a command
  virtual void executeCommand(const QString& Command);

protected:
  ctkConsole(ctkConsolePrivate * pimpl, QWidget* parentObject);

  QScopedPointer<ctkConsolePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkConsole);
  Q_DISABLE_COPY(ctkConsole);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkConsole::EditorHints);

//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkConsoleCompleter : public QCompleter
{
public:

  /// Update the completion model given a string.  The given string
  /// is the current console text between the cursor and the start of
  /// the line.
  virtual void updateCompletionModel(const QString& str) = 0;

  /// Given the current completion, returns the number by which the
  /// cursor should be shifted to the left.
  virtual int cursorOffset(const QString& completion) = 0;

  /// Returns the autocomplete preference list
  QStringList autocompletePreferenceList();

  /// Set autocomplete preference list
  /// This list allows to specify options that could be
  /// selected by default if multiple choices are proposed to the user.
  void setAutocompletePreferenceList(const QStringList& list);

protected:

  QStringList AutocompletePreferenceList;
};


#endif
