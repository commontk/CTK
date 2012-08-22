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

/**
 * \class ctkCmdLineModuleXmlProgressWatcher
 * \brief Provides progress updates of a module.
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXmlProgressWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleXmlProgressWatcher(QIODevice* input);
  ~ctkCmdLineModuleXmlProgressWatcher();

Q_SIGNALS:

  void filterStarted(const QString& name, const QString& comment);
  void filterProgress(float progress);
  void filterFinished(const QString& name);
  void filterXmlError(const QString& error);

private:

  friend class ctkCmdLineModuleXmlProgressWatcherPrivate;

  Q_PRIVATE_SLOT(d, void _q_readyRead())

  QScopedPointer<ctkCmdLineModuleXmlProgressWatcherPrivate> d;
};

#endif // CTKCMDLINEMODULEXMLPROGRESSWATCHER_H
