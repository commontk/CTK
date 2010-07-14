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


#ifndef CTKPLUGINGENERATORMAINSECTION_H
#define CTKPLUGINGENERATORMAINSECTION_H

#include "ctkPluginGeneratorAbstractUiSection.h"

#include "ui_ctkPluginGeneratorMainSection.h"

class ctkPluginGeneratorMainSection : public ctkPluginGeneratorAbstractUiSection
{
  Q_OBJECT

public:
    ctkPluginGeneratorMainSection();

protected slots:

    void verifySection();

protected:

    QWidget* createWidget(QWidget* parent);

    void connectSignals();

    Ui::ctkPluginGeneratorMainSection* ui;
};

#endif // CTKPLUGINGENERATORMAINSECTION_H
