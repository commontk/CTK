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

#ifndef CTKCMDLINEMODULEXMLPROGRESSWATCHER_H
#define CTKCMDLINEMODULEXMLPROGRESSWATCHER_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QObject>

class ctkCmdLineModuleXmlProgressWatcherPrivate;

class QIODevice;
class QProcess;

/**
 * \class ctkCmdLineModuleXmlProgressWatcher
 * \brief Provides progress updates of a module.
 * \ingroup CommandLineModulesCore_API
 *
 * This class is usually only used by back-end implementators for modules
 * which can report progress and results in the form of XML fragments written
 * to a QIODevice.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXmlProgressWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleXmlProgressWatcher(QIODevice* input);
  ctkCmdLineModuleXmlProgressWatcher(QProcess* input);
  ~ctkCmdLineModuleXmlProgressWatcher();

Q_SIGNALS:

  void filterStarted(const QString& name, const QString& comment);
  void filterProgress(float progress, const QString& comment);
  void filterResult(const QString& parameter, const QString& value);
  void filterFinished(const QString& name, const QString& comment);
  void filterXmlError(const QString& error);

  void outputDataAvailable(const QByteArray& outputData);
  void errorDataAvailable(const QByteArray& errorData);

private:

  friend class ctkCmdLineModuleXmlProgressWatcherPrivate;

  Q_PRIVATE_SLOT(d, void _q_readyRead())
  Q_PRIVATE_SLOT(d, void _q_readyReadError())

  QScopedPointer<ctkCmdLineModuleXmlProgressWatcherPrivate> d;
};

#endif // CTKCMDLINEMODULEXMLPROGRESSWATCHER_H
