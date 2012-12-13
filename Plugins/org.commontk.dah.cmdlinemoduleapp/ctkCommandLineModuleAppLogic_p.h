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


#ifndef CTKCOMMANDLINEMODULEAPPLOGIC_P_H
#define CTKCOMMANDLINEMODULEAPPLOGIC_P_H

// Qt includes
#include <QUuid>

// CTK includes
#include <ctkDicomAbstractApp.h>
#include <ctkDicomHostInterface.h>
#include <ctkCmdLineModuleManager.h>

#include <ctkServiceTracker.h>

// ui of this application
#include "ui_ctkCommandLineModuleAppWidget.h"

struct ctkDicomHostInterface;

class ctkCmdLineModuleFrontend;

class QWidget;

class ctkCommandLineModuleAppLogic : public ctkDicomAbstractApp
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomAppInterface)

public:

  ctkCommandLineModuleAppLogic(const QString & modulelocation);
  virtual ~ctkCommandLineModuleAppLogic();

  // ctkDicomAppInterface

  /**
   * Method triggered by the host. By calling this method, the Hosting System is asking the Hosted Application to take whatever steps are
   * needed to make its GUI visible as the topmost window, and to gain focus.
   * \return TRUE if the Hosted Application received the request and will act on it. Otherwise it returns FALSE
   */
  virtual bool bringToFront(const QRect& requestedScreenArea);

  // some logic
  /** Test function for checking */
  void do_something();


protected Q_SLOTS:

  void onStartProgress();
  void onResumeProgress();
  void onSuspendProgress();
  void onCancelProgress();
  void onExitHostedApp();
  void onReleaseResources();

  void onLoadDataClicked();
  void onCreateSecondaryCapture();

  void onDataAvailable();
private:
  QWidget * AppWidget;
  Ui::ctkCommandLineModuleAppWidget ui;

  QString ModuleLocation;

  ctkCmdLineModuleManager ModuleManager;
  ctkCmdLineModuleFrontend* ModuleFrontend;

  QString OutputLocation;
}; // ctkCommandLineModuleAppLogic

#endif // CTKCOMMANDLINEMODULEAPPLOGIC_P_H
