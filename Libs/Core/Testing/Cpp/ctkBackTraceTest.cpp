/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#include <ctkBackTrace.h>

#include <QDebug>

//-----------------------------------------------------------------------------
void Q_DECL_EXPORT bt_func1()
{
  ctkBackTrace bt;
  QList<QString> trace = bt.stackTrace();
  qDebug() << trace;

  // Search all frames for expected symbols instead of checking fixed indices.
  // Frame ordering varies by platform and compiler (e.g. ARM64 may insert
  // extra constructor thunks that shift frame numbers).
  bool foundBackTrace = false;
  bool foundFunc1 = false;
  bool foundFunc2 = false;
  for (const QString& frame : trace)
  {
    if (frame.contains("ctkBackTrace"))
    {
      foundBackTrace = true;
    }
    if (frame.contains("bt_func1"))
    {
      foundFunc1 = true;
    }
    if (frame.contains("bt_func2"))
    {
      foundFunc2 = true;
    }
  }

  if (!foundBackTrace)
  {
    qCritical() << "Stack frame for ctkBackTrace::ctkBackTrace(...) missing";
    exit(EXIT_FAILURE);
  }

  if (!foundFunc1)
  {
    qCritical() << "Stack frame for bt_func1() missing";
    exit(EXIT_FAILURE);
  }

  if (!foundFunc2)
  {
    qCritical() << "Stack frame for bt_func2() missing";
    exit(EXIT_FAILURE);
  }
}

//-----------------------------------------------------------------------------
void Q_DECL_EXPORT bt_func2()
{
  bt_func1();
}


//-----------------------------------------------------------------------------
int ctkBackTraceTest(int /*argc*/, char* /*argv*/[])
{
  bt_func2();

  return EXIT_SUCCESS;
}
