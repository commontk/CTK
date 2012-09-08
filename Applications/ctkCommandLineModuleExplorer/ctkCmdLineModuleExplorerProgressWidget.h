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

#ifndef CTKCMDLINEMODULEEXPLORERPROGRESSWIDGET_H
#define CTKCMDLINEMODULEEXPLORERPROGRESSWIDGET_H

#include "ctkCmdLineModuleResult.h"
#include "ctkCmdLineModuleFutureWatcher.h"

#include <QWidget>
#include <QFutureWatcher>
#include <QTimer>

class ctkCmdLineModuleFuture;

namespace Ui {
class ctkCmdLineModuleExplorerProgressWidget;
}

/**
 * \class ctkCmdLineModuleExplorerProgressWidget
 * \brief Example application progress update widget.
 */
class ctkCmdLineModuleExplorerProgressWidget : public QWidget
{
  Q_OBJECT
  
public:

  ctkCmdLineModuleExplorerProgressWidget(QWidget *parent = 0);
  ~ctkCmdLineModuleExplorerProgressWidget();

  void setFuture(const ctkCmdLineModuleFuture& future);

  void setTitle(const QString& title);

  void setHighlightStyle(bool highlight);

Q_SIGNALS:

  void clicked();

protected:

  void mouseReleaseEvent(QMouseEvent*);

private Q_SLOTS:

  void on_PauseButton_toggled(bool toggled);
  void on_RemoveButton_clicked();

  void checkModulePaused();

  void moduleStarted();
  void moduleCanceled();
  void moduleFinished();
  void moduleResumed();
  void moduleProgressRangeChanged(int progressMin, int progressMax);
  void moduleProgressTextChanged(const QString& progressText);
  void moduleProgressValueChanged(int progressValue);
  
private:
  Ui::ctkCmdLineModuleExplorerProgressWidget *ui;

  ctkCmdLineModuleFutureWatcher FutureWatcher;
  QTimer PollPauseTimer;

};

#endif // CTKCMDLINEMODULEEXPLORERPROGRESSWIDGET_H
