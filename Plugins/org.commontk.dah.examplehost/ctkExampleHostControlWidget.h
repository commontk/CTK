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

#ifndef ctkExampleHostControlWidget_H
#define ctkExampleHostControlWidget_H

#include <QWidget>
#include <QProcess>

#include <ctkDicomAppHostingTypes.h>

#include <org_commontk_dah_examplehost_Export.h>

class ctkExampleDicomHost;

namespace Ui {
  class ctkExampleHostControlWidget;
}

class org_commontk_dah_examplehost_EXPORT ctkExampleHostControlWidget : public QWidget
{
  Q_OBJECT

public:

  explicit ctkExampleHostControlWidget(ctkExampleDicomHost * host, QWidget *parent = 0);
  virtual ~ctkExampleHostControlWidget();

  bool validAppFileName();
public slots:
  // the following to in capital letters for compatibility to ctkExampleDicomHost
  void StartApplication(QString appFileName="");

  void setAppFileName(QString name);

  void runButtonClicked();
  void stopButtonClicked();
  void suspendButtonClicked();
  void cancelButtonClicked();
  void appProcessError(QProcess::ProcessError error);
  void appProcessStateChanged(QProcess::ProcessState state);
  void appStateChanged(ctkDicomAppHosting::State state);

  void outputMessage();
protected:

  ctkExampleDicomHost* Host;
  QString AppFileName;
private:

  bool ValidAppFileName;

  Ui::ctkExampleHostControlWidget *ui;
};

#endif // CTKHOSTWIDGET_H
