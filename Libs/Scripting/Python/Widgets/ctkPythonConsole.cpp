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
#include <QCoreApplication>
#include <QIcon>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextCharFormat>
#include <QVBoxLayout>

// PythonQt includes
#include <PythonQt.h>
#include <PythonQtObjectPtr.h>

// CTK includes
#include <ctkConsole.h>
#include <ctkConsole_p.h>
#include <ctkAbstractPythonManager.h>
#include "ctkPythonConsole.h"

#ifdef __GNUC__
// Disable warnings related to Python macros and functions
// See http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
// Note: Ideally the incriminated functions and macros should be fixed upstream ...
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

//----------------------------------------------------------------------------
// ctkPythonConsoleCompleter

//----------------------------------------------------------------------------
class ctkPythonConsoleCompleter : public ctkConsoleCompleter
{
public:
  ctkPythonConsoleCompleter(ctkAbstractPythonManager& pythonManager)
    : PythonManager(pythonManager)
    {
    this->setParent(&pythonManager);
    }

  virtual void updateCompletionModel(const QString& completion)
    {
    // Start by clearing the model
    this->setModel(0);

    // Don't try to complete the empty string
    if (completion.isEmpty())
      {
      return;
      }

    // Search backward through the string for usable characters
    QString textToComplete;
    for (int i = completion.length()-1; i >= 0; --i)
      {
      QChar c = completion.at(i);
      if (c.isLetterOrNumber() || c == '.' || c == '_')
        {
        textToComplete.prepend(c);
        }
      else
        {
        break;
        }
      }

    // Split the string at the last dot, if one exists
    QString lookup;
    QString compareText = textToComplete;
    int dot = compareText.lastIndexOf('.');
    if (dot != -1)
      {
      lookup = compareText.mid(0, dot);
      compareText = compareText.mid(dot+1);
      }

    // Lookup python names
    QStringList attrs;
    if (!lookup.isEmpty() || !compareText.isEmpty())
      {
      bool appendParenthesis = true;
      attrs = this->PythonManager.pythonAttributes(lookup, QLatin1String("__main__"), appendParenthesis);
      attrs << this->PythonManager.pythonAttributes(lookup, QLatin1String("__main__.__builtins__"),
                                                    appendParenthesis);
      attrs.removeDuplicates();
      }

    // Initialize the completion model
    if (!attrs.isEmpty())
      {
      this->setCompletionMode(QCompleter::PopupCompletion);
      this->setModel(new QStringListModel(attrs, this));
      this->setCaseSensitivity(Qt::CaseInsensitive);
      this->setCompletionPrefix(compareText.toLower());
      
      //qDebug() << "completion" << completion;
      // If a dot as been entered and if an item of possible
      // choices matches one of the preference list, it will be selected.
      QModelIndex preferredIndex = this->completionModel()->index(0, 0);
      int dotCount = completion.count('.');
      if (dotCount == 0 || completion.at(completion.count() - 1) == '.')
        {
        foreach(const QString& pref, this->AutocompletePreferenceList)
          {
          //qDebug() << "pref" << pref;
          int dotPref = pref.count('.');
          // Skip if there are dots in pref and if the completion has already more dots 
          // than the pref
          if ((dotPref != 0) && (dotCount > dotPref))
            {
            continue;
            }
          // Extract string before the last dot
          int lastDot = pref.lastIndexOf('.');
          QString prefBeforeLastDot;
          if (lastDot != -1)
            {
            prefBeforeLastDot = pref.left(lastDot);
            }
          //qDebug() << "prefBeforeLastDot" << prefBeforeLastDot;
          if (!prefBeforeLastDot.isEmpty() && QString::compare(prefBeforeLastDot, lookup) != 0)
            {
            continue;
            }
          QString prefAfterLastDot = pref;
          if (lastDot != -1 )
            {
            prefAfterLastDot = pref.right(pref.size() - lastDot - 1);
            }
          //qDebug() << "prefAfterLastDot" << prefAfterLastDot;
          QModelIndexList list = this->completionModel()->match(
                this->completionModel()->index(0, 0), Qt::DisplayRole, QVariant(prefAfterLastDot));
          if (list.count() > 0)
            {
            preferredIndex = list.first();
            break;
            }
          }
        }

      this->popup()->setCurrentIndex(preferredIndex);
      }
    }
  ctkAbstractPythonManager& PythonManager;
};

//----------------------------------------------------------------------------
// ctkPythonConsolePrivate

//----------------------------------------------------------------------------
class ctkPythonConsolePrivate : public ctkConsolePrivate
{
  Q_DECLARE_PUBLIC(ctkPythonConsole);
public:
  ctkPythonConsolePrivate(ctkPythonConsole& object);
  ~ctkPythonConsolePrivate();

  void initializeInteractiveConsole();

  bool push(const QString& code);

  /// Reset the input buffer of the interactive console
//  void resetInputBuffer();

  void printWelcomeMessage();

  ctkAbstractPythonManager* PythonManager;

  PyObject*                 InteractiveConsole;
};

//----------------------------------------------------------------------------
// ctkPythonConsolePrivate methods

//----------------------------------------------------------------------------
ctkPythonConsolePrivate::ctkPythonConsolePrivate(ctkPythonConsole& object)
  : ctkConsolePrivate(object), PythonManager(0), InteractiveConsole(0)
{
}

//----------------------------------------------------------------------------
ctkPythonConsolePrivate::~ctkPythonConsolePrivate()
{
}

//----------------------------------------------------------------------------
void ctkPythonConsolePrivate::initializeInteractiveConsole()
{
  Q_ASSERT(this->PythonManager);

  // set up the code.InteractiveConsole instance that we'll use.
  const char* code =
    "import code\n"
    "__ctkConsole = code.InteractiveConsole(locals())\n";
  PyRun_SimpleString(code);

  // Now get the reference to __ctkConsole and save the pointer.
  PyObject* main_module = PyImport_AddModule("__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  this->InteractiveConsole = PyDict_GetItemString(
    global_dict, "__ctkConsole");
  if (!this->InteractiveConsole)
    {
    qCritical("Failed to locate the InteractiveConsole object.");
    }
}

//----------------------------------------------------------------------------
bool ctkPythonConsolePrivate::push(const QString& code)
{
  Q_ASSERT(this->PythonManager);

  bool ret_value = false;

  QString buffer = code;
  // The embedded python interpreter cannot handle DOS line-endings, see
  // http://sourceforge.net/tracker/?group_id=5470&atid=105470&func=detail&aid=1167922
  buffer.remove('\r');

  PyObject *res = PyObject_CallMethod(this->InteractiveConsole,
                                      const_cast<char*>("push"),
                                      const_cast<char*>("z"),
                                      buffer.toLatin1().data());
  if (res)
    {
    int status = 0;
    if (PyArg_Parse(res, "i", &status))
      {
      ret_value = (status > 0);
      }
    Py_DECREF(res);
    }
  return ret_value;
}

////----------------------------------------------------------------------------
//void ctkPythonConsolePrivate::resetInputBuffer()
//{
//  if (this->InteractiveConsole)
//    {
//    //this->MakeCurrent();
//    const char* code = "__ctkConsole.resetbuffer()\n";
//    PyRun_SimpleString(code);
//    //this->ReleaseControl();
//    }
//}

//----------------------------------------------------------------------------
void ctkPythonConsolePrivate::printWelcomeMessage()
{
  Q_Q(ctkPythonConsole);

  q->printMessage(
    QString("Python %1 on %2\n").arg(Py_GetVersion()).arg(Py_GetPlatform()),
    q->welcomeTextColor());
}

//----------------------------------------------------------------------------
// ctkPythonConsole methods

//----------------------------------------------------------------------------
ctkPythonConsole::ctkPythonConsole(QWidget* parentObject):
  Superclass(new ctkPythonConsolePrivate(*this), parentObject)
{
  this->setObjectName("pythonConsole");
  this->setWindowIcon(QIcon(":/python-icon.png"));

  // Disable RemoveTrailingSpaces and AutomaticIndentation
  this->setEditorHints(this->editorHints() ^ (RemoveTrailingSpaces | AutomaticIndentation));

  // Enable SplitCopiedTextByLine
  this->setEditorHints(this->editorHints() | SplitCopiedTextByLine);

  this->setDisabled(true);
}

//----------------------------------------------------------------------------
ctkPythonConsole::~ctkPythonConsole()
{
}

////----------------------------------------------------------------------------
void ctkPythonConsole::initialize(ctkAbstractPythonManager* newPythonManager)
{
  Q_D(ctkPythonConsole);

  if (d->PythonManager)
    {
    qWarning() << "ctkPythonConsole already initialized !";
    return;
    }

  // The call to mainContext() ensures that python has been initialized.
  Q_ASSERT(newPythonManager);
  newPythonManager->mainContext();
  Q_ASSERT(PythonQt::self()); // PythonQt should be initialized

  ctkPythonConsoleCompleter* completer = new ctkPythonConsoleCompleter(*newPythonManager);
  this->setCompleter(completer);

  d->PythonManager = newPythonManager;

  d->initializeInteractiveConsole();

  this->connect(PythonQt::self(), SIGNAL(pythonStdOut(QString)),
                d, SLOT(printOutputMessage(QString)));
  this->connect(PythonQt::self(), SIGNAL(pythonStdErr(QString)),
                d, SLOT(printErrorMessage(QString)));

  PythonQt::self()->setRedirectStdInCallback(
        ctkConsole::stdInRedirectCallBack, reinterpret_cast<void*>(this));

  // Set primary and secondary prompt
  this->setPs1(">>> ");
  this->setPs2("... ");

  this->reset();

  // Expose help() function
  QStringList helpImportCode;
  helpImportCode << "from pydoc import help";
  d->PythonManager->executeString(helpImportCode.join("\n"));

  this->setDisabled(false);
}

////----------------------------------------------------------------------------
//void ctkPythonConsole::executeScript(const QString& script)
//{
//  Q_D(ctkPythonConsole);
//  Q_UNUSED(script);

//  d->printOutputMessage("\n");
//  emit this->executing(true);
////   d->Interpreter->RunSimpleString(
////     script.toLatin1().data());
//  emit this->executing(false);
//  d->promptForInput();
//}

//----------------------------------------------------------------------------
QString ctkPythonConsole::ps1() const
{
  PyObject * ps1 = PySys_GetObject(const_cast<char*>("ps1"));
  const char * ps1_str = PyString_AsString(ps1);
  return QLatin1String(ps1_str);
}

//----------------------------------------------------------------------------
void ctkPythonConsole::setPs1(const QString& newPs1)
{
  PySys_SetObject(const_cast<char*>("ps1"), PyString_FromString(newPs1.toLatin1().data()));
}

//----------------------------------------------------------------------------
QString ctkPythonConsole::ps2() const
{
  PyObject * ps2 = PySys_GetObject(const_cast<char*>("ps2"));
  const char * ps2_str = PyString_AsString(ps2);
  return QLatin1String(ps2_str);
}

//----------------------------------------------------------------------------
void ctkPythonConsole::setPs2(const QString& newPs2)
{
  PySys_SetObject(const_cast<char*>("ps2"), PyString_FromString(newPs2.toLatin1().data()));
}

//----------------------------------------------------------------------------
void ctkPythonConsole::executeCommand(const QString& command)
{
  Q_D(ctkPythonConsole);
  d->MultilineStatement = d->push(command);
}

//----------------------------------------------------------------------------
void ctkPythonConsole::reset()
{
  // Set primary and secondary prompt
  this->setPs1(">>> ");
  this->setPs2("... ");

  this->Superclass::reset();
}

