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


#include "ctkCmdLineModuleExplorerShowXmlAction.h"
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleXmlException.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QTextEdit>
#include <QUrl>
#include <QLabel>

ctkCmdLineModuleExplorerShowXmlAction::ctkCmdLineModuleExplorerShowXmlAction(QObject *parent)
  : QAction(parent)
{
  this->setText("Show XML Description");

  connect(this, SIGNAL(triggered()), SLOT(run()));
}

void ctkCmdLineModuleExplorerShowXmlAction::setModuleReference(const ctkCmdLineModuleReference& ref)
{
  this->ModuleRef = ref;
}

void ctkCmdLineModuleExplorerShowXmlAction::run()
{
  QDialog* dialog = new QDialog();
  try
  {
    dialog->setWindowTitle(this->ModuleRef.description().title());
  }
  catch (const ctkCmdLineModuleXmlException&)
  {
    dialog->setWindowTitle(this->ModuleRef.location().toString());
  }

  dialog->setLayout(new QVBoxLayout());

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch(1);
  QPushButton* closeButton = new QPushButton(tr("Close"), dialog);
  buttonLayout->addWidget(closeButton);

  QTextEdit* textEdit = new QTextEdit(dialog);
  textEdit->setPlainText(this->ModuleRef.rawXmlDescription().data());

  QLabel* statusLabel = new QLabel(dialog);
  statusLabel->setWordWrap(true);
  if (this->ModuleRef.xmlValidationErrorString().isEmpty())
  {
    statusLabel->setText(tr("No validation errors."));
  }
  else
  {
    statusLabel->setText(this->ModuleRef.xmlValidationErrorString());
  }
  dialog->layout()->addWidget(statusLabel);
  dialog->layout()->addWidget(textEdit);
  dialog->layout()->addItem(buttonLayout);

  connect(closeButton, SIGNAL(clicked()), dialog, SLOT(close()));

  dialog->resize(800, 600);
  dialog->show();
}
