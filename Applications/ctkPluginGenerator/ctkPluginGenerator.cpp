/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginGenerator.h"
#include "ui_ctkPluginGeneratorMainWindow.h"


ctkPluginGenerator::ctkPluginGenerator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ctkPluginGeneratorMainWindow)
{
    ui->setupUi(this);

    this->setStatusBar(0);

    connect(ui->cancelButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    //ctkPluginGeneratorAbstractSection* section = new ctkPluginGeneratorMainSection();
    //ui->sectionsStack->addWidget(section->createWidget(ui->sectionsStack));
    //sectionList.append(section);

    //connect(section, SIGNAL(errorMessageChanged(QString)), this, SLOT(sectionErrorMessage(QString)));
}

ctkPluginGenerator::~ctkPluginGenerator()
{
    delete ui;
}

void ctkPluginGenerator::sectionErrorMessage(const QString& errorMsg)
{
  ui->sectionMessageLabel->setText(errorMsg);
}

