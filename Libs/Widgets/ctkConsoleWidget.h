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
/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
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

#ifndef __ctkConsoleWidget_h
#define __ctkConsoleWidget_h

// Qt includes
#include <QWidget>
#include <QTextCharFormat>
#include <QCompleter>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkConsoleWidgetCompleter;


/// Qwidget that provides an interactive console - you can send text to the
/// console by calling printString() and receive user input by connecting to the
/// executeCommand() slot.
///  
/// \sa pqPythonShell, pqOutputWindow
class CTK_WIDGETS_EXPORT ctkConsoleWidget : public QWidget
{
  Q_OBJECT
  
public:
  ctkConsoleWidget(QWidget* parent = 0);
  virtual ~ctkConsoleWidget();

  /// Returns the current formatting that will be used by printString
  QTextCharFormat getFormat();
  
  /// Sets formatting that will be used by printString
  void setFormat(const QTextCharFormat& Format);

  /// Set a completer for this console widget
  void setCompleter(ctkConsoleWidgetCompleter* completer);
  
signals:
  /// Signal emitted whenever the user enters a command
  void executeCommand(const QString& Command);

public slots:
  /// Writes the supplied text to the console
  void printString(const QString& Text);

  /// Updates the current command. Unlike printString, this will affect the
  /// current command being typed.
  void printCommand(const QString& cmd);

  /// Clears the contents of the console
  void clear();

  /// Puts out an input accepting prompt.
  /// It is recommended that one uses prompt instead of printString() to print
  /// an input prompt since this call ensures that the prompt is shown on a new
  /// line.
  void prompt(const QString& text);

  /// Inserts the given completion string at the cursor.  This will replace
  /// the current word that the cursor is touching with the given text.
  /// Determines the word using QTextCursor::StartOfWord, EndOfWord.
  void insertCompletion(const QString& text);

private:
  ctkConsoleWidget(const ctkConsoleWidget&);
  ctkConsoleWidget& operator=(const ctkConsoleWidget&);

  void internalExecuteCommand(const QString& Command);

  class pqImplementation;
  pqImplementation* const Implementation;
  friend class pqImplementation;
};

//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkConsoleWidgetCompleter : public QCompleter
{
public:

  /// Update the completion model given a string.  The given string
  /// is the current console text between the cursor and the start of
  /// the line.
  virtual void updateCompletionModel(const QString& str) = 0;
};


#endif // !__ctkConsoleWidget_h
