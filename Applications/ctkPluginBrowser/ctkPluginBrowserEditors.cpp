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

#include "ctkPluginBrowserEditors.h"

#include <QTextEdit>
#include <QVBoxLayout>

ctkPluginBrowserEditors::ctkPluginBrowserEditors(QWidget* editorArea)
  : QObject(editorArea), tabWidget(editorArea)
{
  editorArea->setLayout(new QVBoxLayout());
  editorArea->layout()->addWidget(&tabWidget);

  tabWidget.setTabsClosable(true);

  connect(&tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequested(int)));
}

void ctkPluginBrowserEditors::openEditor(const QString &location, const QByteArray& content,
                                 const QString& title, const QString& tooltip)
{
  Q_UNUSED(tooltip)

  if (editorLocations.contains(location))
  {
    int index = editorLocations.indexOf(location);
    tabWidget.setCurrentIndex(index);
  }
  else
  {
    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setPlainText(QString(content));
    int index = tabWidget.addTab(textEdit, title);
    editorLocations.insert(index, location);
    tabWidget.setCurrentIndex(index);
  }
}

void ctkPluginBrowserEditors::tabCloseRequested(int index)
{
  editorLocations.removeAt(index);
  tabWidget.removeTab(index);
}
