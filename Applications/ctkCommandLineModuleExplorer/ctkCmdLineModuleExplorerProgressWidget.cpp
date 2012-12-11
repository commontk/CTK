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

#include <ctkCmdLineModuleFuture.h>

#include "ctkCmdLineModuleExplorerProgressWidget.h"
#include "ui_ctkCmdLineModuleExplorerProgressWidget.h"


ctkCmdLineModuleExplorerProgressWidget::ctkCmdLineModuleExplorerProgressWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ctkCmdLineModuleExplorerProgressWidget)
{
  ui->setupUi(this);

  ui->RemoveButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));

  // Due to Qt bug 12152, we cannot listen to the "paused" signal because it is
  // not emitted directly when the QFuture is paused. Instead, it is emitted after
  // resuming the future, after the "resume" signal has been emitted... we use
  // a polling aproach instead.
  PollPauseTimer.setInterval(300);
  connect(&PollPauseTimer, SIGNAL(timeout()), SLOT(checkModulePaused()));

  connect(&FutureWatcher, SIGNAL(started()), SLOT(moduleStarted()));
  connect(&FutureWatcher, SIGNAL(canceled()), SLOT(moduleCanceled()));
  connect(&FutureWatcher, SIGNAL(finished()), SLOT(moduleFinished()));
  connect(&FutureWatcher, SIGNAL(resumed()), SLOT(moduleResumed()));
  connect(&FutureWatcher, SIGNAL(progressRangeChanged(int,int)), SLOT(moduleProgressRangeChanged(int,int)));
  connect(&FutureWatcher, SIGNAL(progressTextChanged(QString)), ui->ProgressText, SLOT(setText(QString)));
  connect(&FutureWatcher, SIGNAL(progressValueChanged(int)), ui->ProgressBar, SLOT(setValue(int)));

  connect(ui->CancelButton, SIGNAL(clicked()), &this->FutureWatcher, SLOT(cancel()));

  PollPauseTimer.start();
}

ctkCmdLineModuleExplorerProgressWidget::~ctkCmdLineModuleExplorerProgressWidget()
{
  delete ui;
}

void ctkCmdLineModuleExplorerProgressWidget::setFuture(const ctkCmdLineModuleFuture &future)
{
  ui->PauseButton->setEnabled(future.canPause());
  ui->CancelButton->setEnabled(future.canCancel());
  ui->RemoveButton->setEnabled(!future.isRunning());

  FutureWatcher.setFuture(future);
}

void ctkCmdLineModuleExplorerProgressWidget::setTitle(const QString &title)
{
  ui->ProgressTitle->setText(title);
}

void ctkCmdLineModuleExplorerProgressWidget::setHighlightStyle(bool highlight)
{
  QPalette::ColorRole labelRole = highlight ? QPalette::NoRole : QPalette::Midlight;

  ui->ProgressTitle->setForegroundRole(labelRole);
  ui->ProgressText->setForegroundRole(labelRole);
  ui->ProgressBar->setEnabled(highlight);
}

void ctkCmdLineModuleExplorerProgressWidget::mouseReleaseEvent(QMouseEvent*)
{
  emit clicked();
}

void ctkCmdLineModuleExplorerProgressWidget::on_PauseButton_toggled(bool toggled)
{
  this->FutureWatcher.setPaused(toggled);
}

void ctkCmdLineModuleExplorerProgressWidget::on_RemoveButton_clicked()
{
  this->deleteLater();
}

void ctkCmdLineModuleExplorerProgressWidget::moduleStarted()
{
  this->ui->ProgressBar->setMaximum(0);
}

void ctkCmdLineModuleExplorerProgressWidget::moduleCanceled()
{
  this->ui->PauseButton->setEnabled(false);
  this->ui->PauseButton->setChecked(false);
  this->ui->CancelButton->setEnabled(false);
  this->ui->RemoveButton->setEnabled(true);
}

void ctkCmdLineModuleExplorerProgressWidget::moduleFinished()
{
  this->ui->PauseButton->setEnabled(false);
  this->ui->PauseButton->setChecked(false);
  this->ui->CancelButton->setEnabled(false);
  this->ui->RemoveButton->setEnabled(true);
}

void ctkCmdLineModuleExplorerProgressWidget::checkModulePaused()
{
  if (this->FutureWatcher.future().isPaused())
  {
    if (!ui->PauseButton->isChecked())
    {
      ui->PauseButton->setChecked(true);
    }
  }
  else
  {
    if (ui->PauseButton->isChecked())
    {
      ui->PauseButton->setChecked(false);
    }
  }
}

void ctkCmdLineModuleExplorerProgressWidget::moduleResumed()
{
  this->ui->PauseButton->setChecked(false);
}

void ctkCmdLineModuleExplorerProgressWidget::moduleProgressRangeChanged(int progressMin, int progressMax)
{
  this->ui->ProgressBar->setMinimum(progressMin);
  this->ui->ProgressBar->setMaximum(progressMax);
}

void ctkCmdLineModuleExplorerProgressWidget::moduleProgressTextChanged(const QString& progressText)
{
  Q_UNUSED(progressText)
}

void ctkCmdLineModuleExplorerProgressWidget::moduleProgressValueChanged(int progressValue)
{
  Q_UNUSED(progressValue)
}
