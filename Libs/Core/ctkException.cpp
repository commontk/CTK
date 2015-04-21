/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#include "ctkException.h"

#include <typeinfo>

#include <QDebug>

// --------------------------------------------------------------------------
ctkException::TraceManipulator::TraceManipulator(const ctkException* e)
  : Exc(e)
{
}

// --------------------------------------------------------------------------
QDebug ctkException::TraceManipulator::print(QDebug dbg) const
{
  if (Exc)
    Exc->printStackTrace(dbg);
  return dbg.maybeSpace();
}

// --------------------------------------------------------------------------
ctkException::ctkException(const QString& msg)
  : Msg(msg), NestedException(0)
{
}

// --------------------------------------------------------------------------
ctkException::ctkException(const QString& msg, const ctkException& cause)
  : Msg(msg), NestedException(cause.clone())
{
}

// --------------------------------------------------------------------------
ctkException::ctkException(const ctkException& exc)
  : std::exception(exc), ctkBackTrace(exc), Msg(exc.Msg)
{
  NestedException = exc.NestedException ? exc.NestedException->clone() : 0;
}

// --------------------------------------------------------------------------
ctkException::~ctkException() throw()
{
  delete NestedException;
}

// --------------------------------------------------------------------------
ctkException& ctkException::operator=(const ctkException& exc)
{
  if (&exc != this)
  {
    delete NestedException;
    Msg = exc.Msg;
    NestedException = exc.NestedException ? exc.NestedException->clone() : 0;
  }
  return *this;
}

// --------------------------------------------------------------------------
const ctkException* ctkException::cause() const throw()
{
  return NestedException;
}

// --------------------------------------------------------------------------
void ctkException::setCause(const ctkException& cause)
{
  delete NestedException;
  NestedException = cause.clone();
}

// --------------------------------------------------------------------------
const char *ctkException::name() const throw()
{
  return "ctkException";
}

// --------------------------------------------------------------------------
const char* ctkException::className() const throw()
{
  return typeid(*this).name();
}

// --------------------------------------------------------------------------
const char* ctkException::what() const throw()
{
  if (WhatMsg.empty())
  {
    WhatMsg = std::string(name());
    if (!Msg.isEmpty())
    {
      WhatMsg += ": ";
      WhatMsg += Msg.toStdString();
    }
  }
  return WhatMsg.c_str();
}

// --------------------------------------------------------------------------
QString ctkException::message() const throw()
{
  return Msg;
}

// --------------------------------------------------------------------------
ctkException::TraceManipulator ctkException::printStackTrace() const
{
  return TraceManipulator(this);
}

// --------------------------------------------------------------------------
ctkException* ctkException::clone() const
{
  return new ctkException(*this);
}

// --------------------------------------------------------------------------
void ctkException::rethrow() const
{
  throw *this;
}

// --------------------------------------------------------------------------
QDebug ctkException::printStackTrace(QDebug dbg) const
{
  QSet<const ctkException*> dejaVu;
  dejaVu.insert(this);

  // Print our stack trace
  dbg.nospace() << this->what() << '\n';
  QList<QString> trace = stackTrace();
  foreach(QString traceElement, trace)
  {
    dbg.nospace() << "\tat " << qPrintable(traceElement) << '\n';
  }

  // Print cause, if any
  if (NestedException)
  {
    NestedException->printEnclosedStackTrace(dbg, trace, "Caused by: ", "", dejaVu);
  }
  return dbg;
}

// --------------------------------------------------------------------------
void ctkException::printEnclosedStackTrace(QDebug dbg, const QList<QString>& enclosingTrace,
                                           const QString& caption, const QString& prefix,
                                           QSet<const ctkException*>& dejaVu)
{
  if (dejaVu.contains(this))
  {
    dbg.nospace() << "\t[CIRCULAR REFERENCE:" << this->what() << "]\n";
  }
  else
  {
    dejaVu.insert(this);
    // Compute number of frames in common between this and enclosing trace
    QList<QString> trace = stackTrace();
    int m = trace.size() - 1;
    int n = enclosingTrace.size() - 1;
    while (m >= 0 && n >=0 && trace[m] == enclosingTrace[n])
    {
      m--; n--;
    }
    int framesInCommon = trace.size() - 1 - m;

    // Print our stack trace
    dbg.nospace() << qPrintable(prefix) << qPrintable(caption) << this->what() << '\n';
    for (int i = 0; i <= m; i++)
    {
      dbg.nospace() << qPrintable(prefix) << "\tat " << qPrintable(trace[i]) << '\n';
    }
    if (framesInCommon != 0)
    {
      dbg.nospace() << qPrintable(prefix) << "\t... " << framesInCommon << " more\n";
    }

    // Print cause, if any
    if (NestedException)
    {
      NestedException->printEnclosedStackTrace(dbg, trace, "Caused by: ", prefix, dejaVu);
    }
  }
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkException& exc)
{
  dbg << exc.what();
  return dbg.maybeSpace();
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkException::TraceManipulator& trace)
{
  return trace.print(dbg);
}

CTK_IMPLEMENT_EXCEPTION(ctkUnsupportedOperationException, ctkException, "ctkUnsupportedOperationException")
CTK_IMPLEMENT_EXCEPTION(ctkRuntimeException, ctkException, "ctkRuntimeException")
CTK_IMPLEMENT_EXCEPTION(ctkInvalidArgumentException, ctkRuntimeException, "ctkInvalidArgumentException")
CTK_IMPLEMENT_EXCEPTION(ctkIllegalStateException, ctkRuntimeException, "ctkIllegalStateException")
