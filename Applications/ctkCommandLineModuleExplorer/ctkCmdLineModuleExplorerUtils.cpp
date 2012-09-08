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

#include "ctkCmdLineModuleExplorerUtils.h"

#include <QPainter>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QMessageBox>

QPixmap ctkCmdLineModuleExplorerUtils::createIconOverlay(const QPixmap &base, const QPixmap &overlay)
{
  QPixmap result(base.width(), base.height());
  result.fill(Qt::transparent);
  QPainter painter(&result);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.drawPixmap(0, 0, base);
  painter.drawPixmap(base.width()/2, base.height()/2,
                     overlay.scaled(base.width()/2, base.height()/2, Qt::KeepAspectRatio));
  return result;
}

void ctkCmdLineModuleExplorerUtils:: messageBoxModuleRegistration(const QStringList& modulePaths,
                                                                 const QList<ctkCmdLineModuleReference>& moduleRefs,
                                                                 ctkCmdLineModuleManager::ValidationMode validationMode)
{
  Q_ASSERT(modulePaths.size() == moduleRefs.size());

  QString errorMsg;
  for(int i = 0; i < modulePaths.size(); ++i)
  {
    if (!moduleRefs.at(i))
    {
      errorMsg += QObject::tr("Failed to register ") + modulePaths.at(i) + "\n\n";
    }
    else if (!moduleRefs.at(i).xmlValidationErrorString().isEmpty() &&
             validationMode == ctkCmdLineModuleManager::STRICT_VALIDATION)
    {
      errorMsg += QObject::tr("Failed to register ") + modulePaths.at(i) + ":\n" + moduleRefs.at(i).xmlValidationErrorString() + "\n\n";
    }
  }

  if (!errorMsg.isEmpty())
  {
    QWidget* widget = QApplication::activeModalWidget();
    if (widget == NULL) widget = QApplication::activeWindow();
    QMessageBox::critical(widget, QObject::tr("Failed to register modules"), errorMsg);
  }
}
