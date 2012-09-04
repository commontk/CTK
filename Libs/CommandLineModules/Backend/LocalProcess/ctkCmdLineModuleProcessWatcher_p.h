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

#ifndef CTKCMDLINEMODULEPROCESSWATCHER_P_H
#define CTKCMDLINEMODULEPROCESSWATCHER_P_H

#include "ctkCmdLineModuleXmlProgressWatcher.h"
#include "ctkCmdLineModuleFutureInterface.h"

#include <QObject>
#include <QFutureWatcher>
#include <QTimer>

class ctkCmdLineModuleResult;

class QProcess;

/**
 * \class ctkCmdLineModuleProcessWatcher
 * \brief Provides progress updates using QFutureWatcher
 * \ingroup CommandLineModulesBackendLocalProcess_API
 */
class ctkCmdLineModuleProcessWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleProcessWatcher(QProcess& process, const QString& location,
                                 ctkCmdLineModuleFutureInterface& futureInterface);

protected Q_SLOTS:

  void filterStarted(const QString& name, const QString& comment);
  void filterProgress(float progress, const QString &comment);
  void filterResult(const QString& parameter, const QString& value);
  void filterFinished(const QString& name, const QString &comment);

  void filterXmlError(const QString& error);

  void pauseProcess();
  void resumeProcess();
  void cancelProcess();

  void outputDataAvailable(const QByteArray& outputData);
  void errorDataAvailable(const QByteArray& errorData);

private:

  int updateProgress(float progress);
  int incrementProgress();

  QProcess& process;
  QString location;
  ctkCmdLineModuleFutureInterface& futureInterface;
  ctkCmdLineModuleXmlProgressWatcher processXmlWatcher;
  QFutureWatcher<ctkCmdLineModuleResult> futureWatcher;
  QTimer pollPauseTimer;
  bool processPaused;
  int progressValue;
};

#endif // CTKCMDLINEMODULEPROCESSWATCHER_P_H
