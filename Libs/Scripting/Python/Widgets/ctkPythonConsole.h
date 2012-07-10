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

#ifndef __ctkPythonConsole_h
#define __ctkPythonConsole_h

// CTK includes
#include <ctkConsole.h>
#include "ctkScriptingPythonWidgetsExport.h"

/// \ingroup Scripting_Python_Widgets
///
///  Qt widget that provides an interactive "shell" interface to an embedded Python interpreter.
///  You can put an instance of ctkPythonConsole in a dialog or a window, and the user will be able
/// to enter Python commands and see their output, while the UI is still responsive.
///
/// \sa ctkConsole

class ctkPythonConsolePrivate;
class ctkAbstractPythonManager;

class CTK_SCRIPTING_PYTHON_WIDGETS_EXPORT ctkPythonConsole : public ctkConsole
{
  Q_OBJECT
  
public:
  typedef ctkConsole Superclass;
  ctkPythonConsole(QWidget* parentObject = 0);
  virtual ~ctkPythonConsole();

  /// Initialize
  void initialize(ctkAbstractPythonManager* newPythonManager);

  /// Returns the string used as primary prompt
  virtual QString ps1() const;

  /// Set the string used as primary prompt
  virtual void setPs1(const QString& newPs1);

  /// Returns the string used as secondary prompt
  virtual QString ps2() const;

  /// Set the string used as secondary prompt
  virtual void setPs2(const QString& newPs2);

public Q_SLOTS:

//  void executeScript(const QString&);

  /// Reset ps1 and ps2, clear the console and print the welcome message
  virtual void reset();

protected:
  virtual void executeCommand(const QString& command);

private:
  Q_DECLARE_PRIVATE(ctkPythonConsole);
  Q_DISABLE_COPY(ctkPythonConsole);
};

#endif

