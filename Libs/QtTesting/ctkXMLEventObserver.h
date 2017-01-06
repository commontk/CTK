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

#ifndef __ctkXMLEventObserver_h
#define __ctkXMLEventObserver_h

// QT includes
#include <QString>
#include <QXmlStreamWriter>
#include <QMainWindow>

// QtTesting includes
#include <pqEventObserver.h>
#include <pqTestUtility.h>

// CTKQtTesting includes
#if !defined(NO_SYMBOL_EXPORT)
# include "ctkQtTestingExport.h"
#else
# define CTK_QTTESTING_EXPORT
#endif


class QTextStream;

//-----------------------------------------------------------------------------
class CTK_QTTESTING_EXPORT ctkXMLEventObserver : public pqEventObserver
{
  Q_OBJECT

public:
  ctkXMLEventObserver(QObject* testUtility);
  ~ctkXMLEventObserver();

  virtual void setStream(QTextStream* stream);

  virtual void onRecordEvent(const QString& widget,
                             const QString& command,
                             const QString& arguments,
                             const int& eventType);

  void recordApplicationSettings();
  void recordApplicationSetting(const QString& startElement,
                                const QString& attribute1,
                                const QString& attribute2,
                                const QString& attribute3);
protected:
  QXmlStreamWriter* XMLStream;
  QString XMLString;
  pqTestUtility* TestUtility;

};

#endif // __ctkXMLEventObserver_h
