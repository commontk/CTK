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

#ifndef CTKCMDLINEMODULEFUTUREWATCHER_H
#define CTKCMDLINEMODULEFUTUREWATCHER_H

#include "ctkCommandLineModulesCoreExport.h"

#include "ctkCmdLineModuleResult.h"
#include "ctkCmdLineModuleFutureInterface.h"

#include <QFutureWatcher>

class ctkCmdLineModuleFuture;
struct ctkCmdLineModuleFutureWatcherPrivate;

/**
 * @ingroup CommandLineModulesCore_API
 *
 * @brief The ctkCmdLineModuleFutureWatcher class provides enhanced monitoring of a
 *        ctkCmdLineModuleFuture using signals and slots.
 *
 * This class enhances the standard QFutureWatcher class by adding the two signals
 * outputDataReady() and errorDataReady(). These signals are fired whenever the watched
 * future reports new output data (usually text written to the standard output channel) or
 * new error data (usually text written to the standard error channel).
 *
 * Use readPendingOutputData() or readPendingErrorData() to get the newly added data.
 *
 * \warning While you could use a QFutureWatcher<ctkCmdLineModuleResult> instance directly (and
 *          provide a ctkCmdLineModuleFuture via QFutureWatcher<ctkCmdLineModuleResult>::setFuture(future)
 *          by virtue of "slicing") this is discouraged. The reason is that a member variable of type
 *          QFutureWatcher<ctkCmdLineModuleResult> will have a different size, depending on the inclusion
 *          of the ctkCmdLineModuleFutureInterface.h header (this header provides a specialization of the
 *          QFutureInterface template which adds a data member). This can lead to subtle heap corruptions.
 *          Since the code compiles with or without the ctkCmdLindeModuleFutureInterface.h inclusion, you
 *          should always use ctkCmdLineModuleFutureWatcher when working with ctkCmdLineModuleFuture objects
 *          to avoid runtime heap corruptions.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFutureWatcher : public QFutureWatcher<ctkCmdLineModuleResult>
{
  Q_OBJECT

public:

  ctkCmdLineModuleFutureWatcher(QObject* parent = 0);

  ~ctkCmdLineModuleFutureWatcher();

  void setFuture(const ctkCmdLineModuleFuture& future);

  ctkCmdLineModuleFuture future() const;

  bool event(QEvent* event);

  QByteArray readPendingOutputData() const;
  QByteArray readPendingErrorData() const;

  QByteArray readAllOutputData() const;
  QByteArray readAllErrorData() const;

Q_SIGNALS:

  void outputDataReady();
  void errorDataReady();

private:

  friend struct ctkCmdLineModuleFutureWatcherPrivate;

  QScopedPointer<ctkCmdLineModuleFutureWatcherPrivate> d;

  const ctkCmdLineModuleFutureInterface& futureInterface() const;
  ctkCmdLineModuleFutureInterface& futureInterface();

  // not imlemented
  void setFuture(const QFuture<ctkCmdLineModuleResult>&);
};

#endif // CTKCMDLINEMODULEFUTUREWATCHER_H
