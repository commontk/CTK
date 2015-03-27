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

// Qt includes
#include <QThread>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkITKErrorLogMessageHandler.h"
#include "ctkUtils.h"

#ifdef __GNUC__
// Disable warnings related to 'itkSmartPointer.h' file
// See http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
// Note: Ideally the incriminated functions and macros should be fixed upstream ...
# pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

// ITK includes
#include <itkObjectFactory.h>
#include <itkOutputWindow.h>

namespace itk
{

// --------------------------------------------------------------------------
// ctkITKOutputWindow

// --------------------------------------------------------------------------
class ctkITKOutputWindow : public OutputWindow
{
public:
  /** Standard class typedefs. */
  typedef ctkITKOutputWindow       Self;
  typedef OutputWindow             Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard New method. */
  itkNewMacro(ctkITKOutputWindow);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ctkITKOutputWindow, OutputWindow);

  ctkITKOutputWindow():MessageHandler(0)
  {}
  ~ctkITKOutputWindow(){}

  virtual void DisplayText(const char*);
  virtual void DisplayErrorText(const char*);
  virtual void DisplayWarningText(const char*);
  virtual void DisplayGenericWarningText(const char*);

  virtual void DisplayDebugText(const char*);

  QString parseText(const QString &text, ctkErrorLogContext &context);

  ctkErrorLogAbstractMessageHandler * MessageHandler;

};

// --------------------------------------------------------------------------
// ctkITKOutputWindow methods

//----------------------------------------------------------------------------
void ctkITKOutputWindow::DisplayText(const char* text)
{
  Q_ASSERT(this->MessageHandler);
  this->MessageHandler->handleMessage(
        ctk::qtHandleToString(QThread::currentThreadId()),
        ctkErrorLogLevel::Info,
        this->MessageHandler->handlerPrettyName(), ctkErrorLogContext(), text);
}

//----------------------------------------------------------------------------
void ctkITKOutputWindow::DisplayErrorText(const char* text)
{
  Q_ASSERT(this->MessageHandler);

  ctkErrorLogContext context;
  this->parseText(text, context);

  this->MessageHandler->handleMessage(
        ctk::qtHandleToString(QThread::currentThreadId()),
        ctkErrorLogLevel::Error,
        this->MessageHandler->handlerPrettyName(), context, text);
}

//----------------------------------------------------------------------------
void ctkITKOutputWindow::DisplayWarningText(const char* text)
{
  Q_ASSERT(this->MessageHandler);

  ctkErrorLogContext context;
  this->parseText(text, context);

  this->MessageHandler->handleMessage(
        ctk::qtHandleToString(QThread::currentThreadId()),
        ctkErrorLogLevel::Warning,
        this->MessageHandler->handlerPrettyName(), context, text);
}

//----------------------------------------------------------------------------
void ctkITKOutputWindow::DisplayGenericWarningText(const char* text)
{
  this->DisplayWarningText(text);
}

//----------------------------------------------------------------------------
void ctkITKOutputWindow::DisplayDebugText(const char* text)
{
  Q_ASSERT(this->MessageHandler);

  ctkErrorLogContext context;
  this->parseText(text, context);

  this->MessageHandler->handleMessage(
        ctk::qtHandleToString(QThread::currentThreadId()),
        ctkErrorLogLevel::Debug,
        this->MessageHandler->handlerPrettyName(), context, text);
}

//----------------------------------------------------------------------------
QString ctkITKOutputWindow::parseText(const QString& text, ctkErrorLogContext& context)
{
  context.Message = text;

  QRegExp contextRegExp("[a-zA-Z\\s]+: In (.+), line ([\\d]+)\\n(.+\\(0x[a-fA-F0-9]+\\))\\:\\s(.*)");
  if (contextRegExp.exactMatch(text))
    {
    context.File = contextRegExp.cap(1);
    context.Category = contextRegExp.cap(3);
    context.Line = contextRegExp.cap(2).toInt();
    context.Message = contextRegExp.cap(4);
    }
  return context.Message;
}

} // End of itk namespace

// --------------------------------------------------------------------------
// ctkITKErrorLogMessageHandlerPrivate

// --------------------------------------------------------------------------
class ctkITKErrorLogMessageHandlerPrivate
{
  Q_DECLARE_PUBLIC(ctkITKErrorLogMessageHandler);
protected:
  ctkITKErrorLogMessageHandler* const q_ptr;
public:
  ctkITKErrorLogMessageHandlerPrivate(ctkITKErrorLogMessageHandler& object);
  ~ctkITKErrorLogMessageHandlerPrivate();

  itk::OutputWindow::Pointer SavedITKOutputWindow;
  itk::ctkITKOutputWindow::Pointer CTKITKOutputWindow;

};

// --------------------------------------------------------------------------
// ctkITKErrorLogMessageHandlerPrivate methods

// --------------------------------------------------------------------------
ctkITKErrorLogMessageHandlerPrivate::
ctkITKErrorLogMessageHandlerPrivate(ctkITKErrorLogMessageHandler& object) : q_ptr(&object)
{
  Q_Q(ctkITKErrorLogMessageHandler);
  this->SavedITKOutputWindow = 0;
  this->CTKITKOutputWindow = itk::ctkITKOutputWindow::New();
  this->CTKITKOutputWindow->MessageHandler = q;
}

// --------------------------------------------------------------------------
ctkITKErrorLogMessageHandlerPrivate::~ctkITKErrorLogMessageHandlerPrivate()
{
  this->SavedITKOutputWindow = 0;
  this->CTKITKOutputWindow = 0;
}

// --------------------------------------------------------------------------
// ctkITKErrorLogMessageHandler methods

// --------------------------------------------------------------------------
QString ctkITKErrorLogMessageHandler::HandlerName = QLatin1String("ITK");

//----------------------------------------------------------------------------
ctkITKErrorLogMessageHandler::ctkITKErrorLogMessageHandler() :
  Superclass(), d_ptr(new ctkITKErrorLogMessageHandlerPrivate(*this))
{
}

//----------------------------------------------------------------------------
ctkITKErrorLogMessageHandler::~ctkITKErrorLogMessageHandler()
{
}

//----------------------------------------------------------------------------
QString ctkITKErrorLogMessageHandler::handlerName()const
{
  return ctkITKErrorLogMessageHandler::HandlerName;
}

//----------------------------------------------------------------------------
void ctkITKErrorLogMessageHandler::setEnabledInternal(bool value)
{
  Q_D(ctkITKErrorLogMessageHandler);
  if (value)
    {
    d->SavedITKOutputWindow = itk::OutputWindow::GetInstance();
    itk::OutputWindow::SetInstance(d->CTKITKOutputWindow);
    }
  else
    {
    Q_ASSERT(d->SavedITKOutputWindow.IsNotNull());
    itk::OutputWindow::SetInstance(d->SavedITKOutputWindow);
    d->SavedITKOutputWindow = 0;
    }
}
