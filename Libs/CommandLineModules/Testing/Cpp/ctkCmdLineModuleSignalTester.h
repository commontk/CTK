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

#ifndef CTKCMDLINEMODULESIGNALTESTER_H
#define CTKCMDLINEMODULESIGNALTESTER_H

#include "ctkCmdLineModuleFutureWatcher.h"

#include <QObject>
#include <QList>

class ctkCmdLineModuleFuture;

class ctkCmdLineModuleSignalTester : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleSignalTester();

  void setFuture(const ctkCmdLineModuleFuture& future);
  ctkCmdLineModuleFutureWatcher* watcher();

  bool checkSignals(const QList<QString>& expectedSignals);
  void dumpSignals(const QList<QString>& expectedSignals);

  QList<ctkCmdLineModuleResult> results() const;

public Q_SLOTS:

  virtual void moduleStarted();
  virtual void moduleFinished();
  virtual void moduleProgressValueChanged(int progress);
  virtual void moduleProgressTextChanged(const QString& text);

  virtual void modulePaused();
  virtual void moduleResumed();
  virtual void moduleCanceled();

  virtual void resultReadyAt(int resultIndex);
  virtual void resultReadyAt(int beginIndex, int endIndex);

  virtual void progressRangeChanged(int minimum, int maximum);
  virtual void progressValueChanged(int progressValue);
  virtual void progressTextChanged(const QString &progressText);

  virtual void outputDataReady();
  virtual void errorDataReady();

private:

  ctkCmdLineModuleFutureWatcher Watcher;
  QList<QString> Events;
  QList<ctkCmdLineModuleResult> Results;
};

#endif // CTKCMDLINEMODULESIGNALTESTER_H
