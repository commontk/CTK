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


#include "ctkPluginGeneratorMainExtension.h"

ctkPluginGeneratorMainExtension::ctkPluginGeneratorMainExtension()
  : ui(0)
{

}


QWidget* ctkPluginGeneratorMainExtension::createWidget(QWidget* parent)
{
  ui = new Ui::ctkPluginGeneratorMainExtension();
  QWidget* container = new QWidget(parent);
  ui->setupUi(container);

  connectSignals();

  return container;
}

void ctkPluginGeneratorMainExtension::connectSignals()
{
  connect(ui->symbolicNameEdit, SIGNAL(textChanged(QString)), this, SLOT(verifySection()));
}

void ctkPluginGeneratorMainExtension::verifySection()
{
  if (ui->symbolicNameEdit->text().isEmpty())
  {
    this->setErrorMessage("The symbolic name cannot be empty");
  }
  else
  {
    this->setErrorMessage("");
  }
}

