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


#include "ctkPluginGeneratorOptionsDialog_p.h"
#include "ui_ctkPluginGeneratorOptionsDialog.h"

#include <ctkPluginGeneratorConstants.h>

#include <QSettings>

ctkPluginGeneratorOptionsDialog::ctkPluginGeneratorOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ctkPluginGeneratorOptionsDialog)
{
    ui->setupUi(this);

    QSettings settings;
    ui->licenseEdit->setPlainText(settings.value(
        ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER).toString());
}

ctkPluginGeneratorOptionsDialog::~ctkPluginGeneratorOptionsDialog()
{
    delete ui;
}

void ctkPluginGeneratorOptionsDialog::accept()
{
  QSettings settings;
  settings.setValue(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER,
                    ui->licenseEdit->toPlainText());

  QDialog::accept();
}
