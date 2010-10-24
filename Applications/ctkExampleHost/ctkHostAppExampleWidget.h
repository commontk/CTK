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

#ifndef CTKHOSTAPPEXAMPLEWIDGET_H
#define CTKHOSTAPPEXAMPLEWIDGET_H

#include <QWidget>
#include <QProcess>

#include <ctkDicomAppHostingTypes.h>

class ctkExampleDicomHost;

namespace Ui {
  class ctkHostAppExampleWidget;
}

class ctkHostAppExampleWidget : public QWidget
{
  Q_OBJECT

public:

  explicit ctkHostAppExampleWidget(QWidget *parent = 0);
  void setAppFileName(QString name);

  virtual ~ctkHostAppExampleWidget();

public slots:

  void loadButtonClicked();
  void startButtonClicked();
  void runButtonClicked();
  void stopButtonClicked();
  void appProcessError(QProcess::ProcessError error);
  void appProcessStateChanged(QProcess::ProcessState state);
  void appStateChanged(ctkDicomAppHosting::State state);

  void placeholderResized();

protected:

  ctkExampleDicomHost* host;
  QString appFileName;

private:

  Ui::ctkHostAppExampleWidget *ui;

};

#endif // CTKHOSTWIDGET_H
