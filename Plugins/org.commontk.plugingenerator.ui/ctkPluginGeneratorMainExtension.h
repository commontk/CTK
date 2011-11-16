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


#ifndef CTKPLUGINGENERATORMAINEXTENSION_H
#define CTKPLUGINGENERATORMAINEXTENSION_H

#include "ctkPluginGeneratorAbstractUiExtension.h"

#include "ui_ctkPluginGeneratorMainExtension.h"

class ctkPluginGeneratorMainExtension : public ctkPluginGeneratorAbstractUiExtension
{
  Q_OBJECT

public:
    ctkPluginGeneratorMainExtension();

protected Q_SLOTS:

    void updateParameters();
    void symbolicNameChanged();
    void activatorClassChanged();

protected:

    bool verifyParameters(const QHash<QString, QVariant>& params);
    void updateCodeModel(const QHash<QString, QVariant>& params);

    QWidget* createWidget();

    void connectSignals();

    Ui::ctkPluginGeneratorMainExtension* ui;

};

#endif // CTKPLUGINGENERATORMAINEXTENSION_H
